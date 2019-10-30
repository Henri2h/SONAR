#ifndef SONAR_H
#define SONAR_H

#include "audioPlayer.h"
#include "audioRecorder.h"

#include "chrip.h"
#include "threadrec.h"

class SONAR
{
public:
    SONAR();
    void getDistance();
    void startSound();
    QList<float> getResults();

private:
    AudioPlayer outputPlayer;
    AudioRecorder inputPlayer;
    threadrec threadRec;
    Chrip chrip;

private slots:
    void startRecording();
};

#endif // SONAR_H
