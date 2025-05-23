﻿#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QWidget>
#include <SDL.h>
#include <QFile>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat pixelFormat;
    int fps;
} Yuv;

class YuvPlayer : public QWidget
{
    Q_OBJECT
public:
    typedef enum {
        Stopped = 0,
        Playing,
        Paused,
        Finished
    } State;

    explicit YuvPlayer(QWidget *parent = nullptr);
    ~YuvPlayer();

    void play();
    void pause();
    void stop();
    bool isPlaying();
    State getState();

    void setYuv(const Yuv &yuv);
protected:
    void timerEvent(QTimerEvent *event);

signals:

private:
    Yuv m_yuv;
    int m_timerid = 0;
    SDL_Window *m_window = nullptr;
    SDL_Renderer *m_renderer = nullptr;
    SDL_Texture *m_texture = nullptr;
    QFile m_file;
    State m_state = Stopped;


};

#endif // YUVPLAYER_H
