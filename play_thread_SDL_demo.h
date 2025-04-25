#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QThread>

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
};

#endif // PLAYTHREAD_H
