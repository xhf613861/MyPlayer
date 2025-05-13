#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <SDL.h>
#include "play_thread.h"
#include "play_thread_SDL_YUV_demo.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}



#define FILE_NAME "D:\\video.yuv"
#define PIXEL_FORMAT SDL_PIXELFORMAT_YV12
#define IMG_W 352
#define IMG_H 288

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_player = new YuvPlayer(this);
    int w = 300;
    int h = 200;
    int x = (width() - w) >> 1;
    int y = (height() - h) >> 1;
    m_player->setGeometry(x, y, w, h);

    // 设置需要播放的文件
    Yuv yuv = {
        "D:\\video.yuv",
        IMG_W, IMG_H,
        AV_PIX_FMT_YUV420P,
        30
    };
    m_player->setYuv(yuv);

    connect(m_player, &YuvPlayer::stateChanged, this, [this](){
        if (m_player->getState() == YuvPlayer::Playing) {
            ui->pushButton->setText(QStringLiteral("暂停"));
        } else {
            ui->pushButton->setText(QStringLiteral("播放"));
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
//    SDLYUVPlayThread *play = new SDLYUVPlayThread();
//    play->start();
    if (m_player->isPlaying()) {
        m_player->pause();
    } else {
        m_player->play();
    }
}

void MainWindow::on_stopButton_clicked()
{
    m_player->stop();
}
