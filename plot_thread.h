#ifndef PLOT_THREAD_H
#define PLOT_THREAD_H


#include <QtGui>

class MyThread : public QThread
{
    Q_OBJECT

public:
    MyThread(int iter, QObject *parent);
    void run(); // this is virtual method, we must implement it in our subclass of QThread
private:
    int totiter;
signals:
    void freqChanged(int i);
};

#endif // PLOT_THREAD_H
