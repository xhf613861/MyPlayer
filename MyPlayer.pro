QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

########
FFMPEF_SOURCE_DIR = $$PWD\\third_party\\ffmpeg\\bin\\*
DEST_DIR = ""
FFMPEF_SOURCE_DIR = $$replace(FFMPEF_SOURCE_DIR, '/', '\\')

SDL_SOURCE_DIR = ""
CONFIG(debug, debug|release) {
    DEST_DIR = $$OUT_PWD\\debug\\
} else {
    DEST_DIR = $$OUT_PWD\\release\\
}

DEST_DIR = $$replace(DEST_DIR, '/', '\\')

contains(DEFINES, WIN64) {
    SDL_SOURCE_DIR = $$PWD\\third_party\\SDL2\\lib\\x64
} else {
    SDL_SOURCE_DIR = $$PWD\\third_party\\SDL2\\lib\\x86
}
SDL_SOURCE_DIR = $$replace(SDL_SOURCE_DIR, '/', '\\')

unix {
    QMAKE_POST_LINK += cp -r $$FFMPEF_SOURCE_DIR $$DESTDIR
}
win32 {
    QMAKE_POST_LINK = xcopy $$FFMPEF_SOURCE_DIR $$DEST_DIR /E /I /Y
    QMAKE_POST_LINK = xcopy $$SDL_SOURCE_DIR\\SDL2.dll $$DEST_DIR /E /I /Y
}

INCLUDEPATH += $$PWD/third_party/ffmpeg/include
INCLUDEPATH += $$PWD/third_party/SDL2/include

LIBS += -L$$PWD/third_party/ffmpeg/lib -lswresample -lswscale -lavcodec -lavformat -lavutil -lavdevice -lavfilter
LIBS += -L$$SDL_SOURCE_DIR -lSDL2

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    play_thread.cpp

HEADERS += \
    mainwindow.h \
    play_thread.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
