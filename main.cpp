#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>
#include <SDL.h>
#include "ffmpegs.h"

#undef main

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        return 0;
    }

    //RawVideoFile in = {
    //    "D:\\video.yuv",
    //    352, 288, AV_PIX_FMT_YUV420P
    //};
    //RawVideoFile out = {
    //    "D:\\video.rgb",
    //    352, 100, AV_PIX_FMT_RGB24
    //};
    //FFmpegs::convertRawVideo(in, out);

    QApplication a(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif
    MainWindow w;
    w.show();
    int ret = a.exec();
    SDL_Quit();
    return ret;
}
