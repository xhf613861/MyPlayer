#include "play_thread.h"

#include <SDL.h>
#include <QDebug>
#include <QFile>

#define END(judge, func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        goto end; \
    }

#define FILE_NAME "D:\\3.yuv"
#define PIXEL_FORMAT SDL_PIXELFORMAT_IYUV
#define IMG_W 1920
#define IMG_H 1080

PlayThread::PlayThread(QObject *parent) : QThread(parent)
{
    connect(this, &PlayThread::finished, this, &PlayThread::deleteLater);
}

PlayThread::~PlayThread()
{
    disconnect();
    requestInterruption();
    quit();
    wait();
    qDebug() << this << "析构了";
}

SDL_Texture *PlayThread::createTexture(SDL_Renderer *renderer)
{
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                      SDL_PIXELFORMAT_ARGB32,
                      SDL_TEXTUREACCESS_TARGET,
                      50, 50);

    if (!texture) return nullptr;

    // 设置纹理为渲染目标
    if (SDL_SetRenderTarget(renderer, texture)) return nullptr;
    if (SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_TRANSPARENT)) return nullptr;
    if (SDL_RenderClear(renderer)) return nullptr;

    // 设置颜色
    if (SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE)) return nullptr;

    // 画图形
    SDL_Rect rect = {0, 0, 50, 50};
    if (SDL_RenderDrawRect(renderer, &rect)) return nullptr;

    if (SDL_RenderDrawLine(renderer, 0, 0, 50, 50)) return nullptr;
    if (SDL_RenderDrawLine(renderer, 50, 0, 0, 50)) return nullptr;

    return texture;
}

void PlayThread::showClick(SDL_Renderer *renderer, const SDL_Event &event, SDL_Texture *texture)
{
    SDL_MouseButtonEvent btn = event.button;
    int x = btn.x;
    int y = btn.y;
    SDL_RenderClear(renderer);
    SDL_Rect dstRect = {x, y, 50, 50};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    SDL_RenderPresent(renderer);
}

void PlayThread::run()
{
    // 窗口
    SDL_Window *window = nullptr;

    // 渲染上下文
    SDL_Renderer *renderer = nullptr;

    // 纹理（直接跟特定驱动程序相关的像素数据）
    SDL_Texture *texture = nullptr;

    QFile file(FILE_NAME);


    END(SDL_Init(SDL_INIT_VIDEO), SDL_Init);


    window = SDL_CreateWindow(
                QStringLiteral("SDL修改渲染YUV").toStdString().c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                IMG_W / 2,
                IMG_H / 2,
                SDL_WINDOW_SHOWN
                );
    END(!window, SDL_CreateWindow);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        qDebug() << "不支持硬件加速";
        renderer = SDL_CreateRenderer(window, -1, 0);
    }
    END(!renderer, SDL_CreateRenderer);

    texture = SDL_CreateTexture(renderer,
                                PIXEL_FORMAT,
                                SDL_TEXTUREACCESS_STREAMING,
                                IMG_W, IMG_H);
    END(!texture, SDL_CreateTexture);

    // 将YUV的像素数据填充
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "open error";
        goto end;
    }

    END(SDL_UpdateTexture(texture, nullptr, file.readAll().data(), IMG_W), SDL_UpdateTexture);

    END(SDL_SetRenderTarget(renderer, nullptr), SDL_SetRenderTarget);

    // 设置绘制颜色（画笔颜色）
    END(SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE), SDL_SetRenderDrawColor);

    // 用绘制颜色（画笔颜色）清除目标
    END(SDL_RenderClear(renderer), SDL_RenderClear);

    END(SDL_RenderCopy(renderer, texture, nullptr, nullptr), SDL_RenderCopy);

    SDL_RenderPresent(renderer);

    while (!isInterruptionRequested()) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        qDebug() << QStringLiteral("SDL 事件：") << event.type;
        switch (event.type) {
        case SDL_QUIT: {
            goto end;
            break;
        }
        }
    }

end:
    file.close();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
