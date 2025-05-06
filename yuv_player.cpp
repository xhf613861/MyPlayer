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
    m_file.close();
    freeCurrentImage();
}

void YuvPlayer::play()
{
    m_timerid = startTimer(1000 / 30);
    m_state = Playing;
}

void YuvPlayer::pause()
{
    if (m_timerid) {
        killTimer(m_timerid);
    }
    m_state = Paused;
}

void YuvPlayer::stop()
{
    if (m_timerid) {
        killTimer(m_timerid);
    }
    m_state = Stopped;
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

    // 将YUV的像素数据填充
    m_file.setFileName(yuv.filename);
    if (!m_file.open(QFile::ReadOnly)) {
        qDebug() << "open error";
    }
}

void YuvPlayer::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    int imgSize = av_image_get_buffer_size(m_yuv.pixelFormat, m_yuv.width, m_yuv.height, 1);
    char *data = (char *)malloc(imgSize);
    if (m_file.read(data, imgSize) > 0) {
        RawVideoFrame in = {
            data,
            m_yuv.width, m_yuv.height,
            m_yuv.pixelFormat
        };
        RawVideoFrame out = {
            nullptr,
            m_yuv.width, m_yuv.height,
            AV_PIX_FMT_RGB24
        };

        FFmpegs::convertRawVideo(in, out);
        freeCurrentImage();
        m_currentImage = new QImage((uchar *)out.pixels, out.width, out.height, QImage::Format_RGB888);
        update();
    } else {
        killTimer(m_timerid);
    }
    delete data;
}

void YuvPlayer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (!m_currentImage)
        return;

    QPainter(this).drawImage(QRect(0, 0, width(), height()), *m_currentImage);
}

void YuvPlayer::freeCurrentImage()
{
    if (m_currentImage) {
        free(m_currentImage->bits());
        delete m_currentImage;
    }
}
