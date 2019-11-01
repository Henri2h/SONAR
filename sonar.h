#ifndef SONAR_H
#define SONAR_H

#include "audioPlayer.h"
#include "audioRecorder.h"

#include "chrip.h"

class SONAR
{
public:
    SONAR();
    QList<float> getDistance();
    void startSound();
    QList<float> getResults();

    static const int air_speed = 343; // m/s
    static const int max_distance = 4; // meters

private:
    AudioPlayer outputPlayer;
    AudioRecorder inputPlayer;
    Chrip chrip;

    int sample_desired_length;

    QList<float> sound;

    // distance related functions
    QList<float> correlateToDistance();
    int getStart(QList<float> data);
    QList<float> getDistance(QList<float> data);

private slots:
    void startRecording();
};

#endif // SONAR_H
