#include "plot_thread.h"

MyThread::MyThread(int iter, QObject *parent)
: QThread(parent)
{
    totiter=iter;
}

void MyThread::run()
{
    emit freqChanged(0);
        exec();
}
