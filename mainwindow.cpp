#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <SDL.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    avformat_network_init();  // 网络协议支持初始化
    qDebug() << "FFmpeg version: " << av_version_info();
    SDL_version version;
    SDL_VERSION(&version);
    qDebug() << version.major;
    qDebug() << version.minor;
}

MainWindow::~MainWindow()
{
    delete ui;
}

