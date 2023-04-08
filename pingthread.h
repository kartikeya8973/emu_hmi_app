#ifndef PINGTHREAD_H
#define PINGTHREAD_H

#include <QThread>

class PingThread : public QThread
{
public:
    explicit PingThread(QObject *parent = nullptr);

    void run();

signals:
    void pingResult(QList<int> result);

    void vectorSent(QVector<int>);

};

#endif // PINGTHREAD_H
