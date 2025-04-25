#include "play_thread.h"

#include <SDL.h>
#include <QDebug>
#include <QFile>

#define END(judge, func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        goto end; \
    }


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

void PlayThread::run()
{
    // 像素数据
    SDL_Surface *surface = nullptr;

    // 窗口
    SDL_Window *window = nullptr;

    // 渲染上下文
    SDL_Renderer *renderer = nullptr;

    // 纹理（直接跟特定驱动程序相关的像素数据）
    SDL_Texture *texture = nullptr;

    SDL_Rect srcRect = {200, 200, 100, 100};
    SDL_Rect dstRect = {200, 200, 100, 100};
//    SDL_Rect rect = {0, 0, 50, 50};

    END(SDL_Init(SDL_INIT_VIDEO), SDL_Init);

    surface = SDL_LoadBMP("D:\\1.bmp");

    END(!surface, SDL_LoadBMP);

    window = SDL_CreateWindow(
                "SDL显示BMP图片",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                surface->w,
                surface->h,
                SDL_WINDOW_SHOWN
                );
    END(!window, SDL_CreateWindow);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        qDebug() << "不支持硬件加速";
        renderer = SDL_CreateRenderer(window, -1, 0);
    }
    END(!renderer, SDL_CreateRenderer);

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    END(!texture, SDL_CreateTextureFromSurface);

//    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
//    SDL_RenderFillRect(renderer, &rect);

    // 设置绘制颜色（画笔颜色）
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);

    // 用绘制颜色（画笔颜色）清除目标
    SDL_RenderClear(renderer);

//    END(SDL_RenderCopy(renderer, texture, nullptr, nullptr), SDL_RenderCopy);
    END(SDL_RenderCopy(renderer, texture, &srcRect, &dstRect), SDL_RenderCopy);

    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

end:
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
