#include "yuv_player.h"
#include <QDebug>
#include <map>

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
    if (PIXEL_FORMAT_MAP.size() == 0) {
        PIXEL_FORMAT_MAP[AV_PIX_FMT_YUV420P] = SDL_PIXELFORMAT_IYUV;
        PIXEL_FORMAT_MAP[AV_PIX_FMT_YUYV422] = SDL_PIXELFORMAT_YUY2;
        PIXEL_FORMAT_MAP[AV_PIX_FMT_NONE] = SDL_PIXELFORMAT_UNKNOWN;
    }

    m_window = SDL_CreateWindowFrom((void *)(winId()));
    RET(!m_window, SDL_CreateWindow);

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        qDebug() << "不支持硬件加速";
        m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    }
    RET(!m_renderer, SDL_CreateRenderer);
}

YuvPlayer::~YuvPlayer()
{
    m_file.close();
    SDL_DestroyTexture(m_texture);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void YuvPlayer::play()
{
    if (m_timerid) {
        killTimer(m_timerid);
    }
    m_timerid = startTimer(1000 / 30);
}

void YuvPlayer::pause()
{

}

void YuvPlayer::stop()
{

}

bool YuvPlayer::isPlaying()
{
    return false;
}

void YuvPlayer::setYuv(const Yuv &yuv)
{
    m_yuv = yuv;
    m_texture = SDL_CreateTexture(m_renderer,
                                PIXEL_FORMAT_MAP[yuv.pixelFormat],
                                SDL_TEXTUREACCESS_STREAMING,
                                yuv.width, yuv.height);
    RET(!m_texture, SDL_CreateTexture);

    // 将YUV的像素数据填充
    m_file.setFileName(yuv.filename);
    if (!m_file.open(QFile::ReadOnly)) {
        qDebug() << "open error";
    }
}

void YuvPlayer::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    int imgSize = m_yuv.width * m_yuv.height * 1.5;
    char *data = (char *)malloc(imgSize);
    if (m_file.read(data, imgSize) > 0) {
        RET(SDL_UpdateTexture(m_texture, nullptr, data, m_yuv.width), SDL_UpdateTexture);

        RET(SDL_SetRenderTarget(m_renderer, nullptr), SDL_SetRenderTarget);

        // 设置绘制颜色（画笔颜色）
        RET(SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, SDL_ALPHA_OPAQUE), SDL_SetRenderDrawColor);

        // 用绘制颜色（画笔颜色）清除目标
        RET(SDL_RenderClear(m_renderer), SDL_RenderClear);

        RET(SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr), SDL_RenderCopy);

        SDL_RenderPresent(m_renderer);
    } else {
        killTimer(m_timerid);
    }
}
