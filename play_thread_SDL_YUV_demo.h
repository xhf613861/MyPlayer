#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QThread>
#include <SDL.h>

class PlayThread : public QThread
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = nullptr);
    ~PlayThread();

signals:


    // QThread interface
protected:
    void run();

private:
    SDL_Texture *createTexture(SDL_Renderer *renderer);
    void showClick(SDL_Renderer *renderer, const SDL_Event &event, SDL_Texture *texture);
};

#endif // PLAYTHREAD_H
