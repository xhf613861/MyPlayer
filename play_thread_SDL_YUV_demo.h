#ifndef SDLYUVPLAYTHREAD_H
#define SDLYUVPLAYTHREAD_H

#include <QThread>
#include <SDL.h>

class SDLYUVPlayThread : public QThread
{
    Q_OBJECT
public:
    explicit SDLYUVPlayThread(QObject *parent = nullptr);
    ~SDLYUVPlayThread();

signals:


    // QThread interface
protected:
    void run();

private:
    SDL_Texture *createTexture(SDL_Renderer *renderer);
    void showClick(SDL_Renderer *renderer, const SDL_Event &event, SDL_Texture *texture);
};

#endif // SDLYUVPLAYTHREAD_H
