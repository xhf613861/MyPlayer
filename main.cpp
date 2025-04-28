#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>
#include <SDL.h>

#undef main

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        return 0;
    }
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
