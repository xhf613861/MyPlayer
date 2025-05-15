#include "yuv_player.h"
#include <QDebug>
#include <map>
#include <QPainter>
#include "ffmpegs.h"

#define RET(judge, func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        return; \
    }

static std::map<AVPixelFormat, SDL_PixelFormatEnum> PIXEL_FORMAT_MAP;/* = {
    {AV_PIX_FMT_YUV420P, SDL_PIXELFORMAT_IYUV},
    {AV_PIX_FMT_YUYV422, SDL_PIXELFORMAT_YUY2},
    {AV_PIX_FMT_NONE, SDL_PIXELFORMAT_UNKNOWN}
};*/



YuvPlayer::YuvPlayer(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: black");
    PIXEL_FORMAT_MAP[AV_PIX_FMT_YUV420P] = SDL_PIXELFORMAT_IYUV;
    PIXEL_FORMAT_MAP[AV_PIX_FMT_YUYV422] = SDL_PIXELFORMAT_YUY2;
    PIXEL_FORMAT_MAP[AV_PIX_FMT_NONE] = SDL_PIXELFORMAT_UNKNOWN;
}

YuvPlayer::~YuvPlayer()
{
    closeFile();
    freeCurrentImage();
    stopTimer();
}

void YuvPlayer::setState(YuvPlayer::State state)
{
    if (m_state == state)
        return;

    if (state == Stopped || state == Finished) {
        m_file->seek(0);
    }

    m_state = state;
    emit stateChanged();
}

void YuvPlayer::stopTimer()
{
    if (m_timerid == 0) return;

    killTimer(m_timerid);
    m_timerid = 0;
}

void YuvPlayer::closeFile()
{
    if (!m_file) return;

    m_file->close();
    delete m_file;
    m_file = nullptr;
}

void YuvPlayer::play()
{
    if (m_state == Playing)
        return;

    m_timerid = startTimer(m_interval);
    setState(Playing);
}

void YuvPlayer::pause()
{
    if (m_state != Playing)
        return;

    stopTimer();
    setState(Paused);
}

void YuvPlayer::stop()
{
    if (m_state == Stopped)
        return;

    stopTimer();
    freeCurrentImage();
    update();
    setState(Stopped);
}

bool YuvPlayer::isPlaying()
{
    return m_state == Playing;
}

YuvPlayer::State YuvPlayer::getState()
{
    return m_state;
}

void YuvPlayer::setYuv(const Yuv &yuv)
{
    m_yuv = yuv;
    closeFile();

    // 将YUV的像素数据填充
    m_file = new QFile(yuv.filename);
    m_file->setFileName(yuv.filename);
    if (!m_file->open(QFile::ReadOnly)) {
        qDebug() << "open error " << yuv.filename;
    }

    m_interval = 1000 / m_yuv.fps;
    m_imgSize = av_image_get_buffer_size(m_yuv.pixelFormat, m_yuv.width, m_yuv.height, 1);

    int w = width();
    int h = height();

    int dx = 0;
    int dy = 0;
    int dw = m_yuv.width;
    int dh = m_yuv.height;

    // 计算目标尺寸
    if (dw > w || dh > h) {
        if (dw * h > w *dh) {
            // 视频宽高比大于播放器
            dh = w * dh / dw;
            dw = w;
        } else {
            dw = h * dw / dh;
            dh = h;
        }
    }

    dx = (w - dw) >> 1;
    dy = (h - dh) >> 1;
    m_dstRect = QRect(dx, dy, dw, dh);
}

void YuvPlayer::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    char *data = (char *)malloc(m_imgSize);
    if (m_file->read(data, m_imgSize) == m_imgSize) {
        RawVideoFrame in = {
            data,
            m_yuv.width, m_yuv.height,
            m_yuv.pixelFormat
        };
        RawVideoFrame out = {
            nullptr,
            m_yuv.width >> 4 << 4, m_yuv.height >> 4 << 4,
            AV_PIX_FMT_RGB24
        };

        FFmpegs::convertRawVideo(in, out);
        freeCurrentImage();
        m_currentImage = new QImage((uchar *)out.pixels, out.width, out.height, QImage::Format_RGB888);
        update();
    } else {
        stopTimer();
        setState(Finished);
    }
    delete data;
}

void YuvPlayer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (!m_currentImage)
        return;

    QPainter(this).drawImage(m_dstRect, *m_currentImage);
}

void YuvPlayer::freeCurrentImage()
{
    if (m_currentImage) {
        free(m_currentImage->bits());
        delete m_currentImage;
        m_currentImage = nullptr;
    }
}

