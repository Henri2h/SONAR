#include "threadrec.h"
#include <QDebug>

threadrec::threadrec()
{

}

// hellothread/hellothread.cpp
void threadrec::run()
{
     qDebug() << "hello from worker thread " << thread()->currentThreadId();
     inputPlayer.Record(2);
}
