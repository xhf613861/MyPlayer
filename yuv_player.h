#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QWidget>
#include <SDL.h>
#include <QFile>
#include <QImage>

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
    void paintEvent(QPaintEvent *event);

signals:
    void stateChanged();

private:
    void freeCurrentImage();
    void setState(State state);
    void stopTimer();
    void closeFile();

private:
    Yuv m_yuv;
    int m_timerid = 0;
    QFile *m_file = nullptr;
    State m_state = Stopped;
    QImage *m_currentImage = nullptr;
    int m_imgSize = 0;
    int m_interval = 0;

    QRect m_dstRect;
};

#endif // YUVPLAYER_H
