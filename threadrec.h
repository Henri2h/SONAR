#ifndef THREADREC_H
#define THREADREC_H

#include  <QThread>
#include "audioRecorder.h"

class threadrec : public QThread
{
public:
    threadrec();

private:
    AudioRecorder inputPlayer;
    void run();
};
#endif // THREADREC_H
