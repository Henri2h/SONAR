#ifndef SONAR_H
#define SONAR_H

#include "audioPlayer.h"
#include "audioRecorder.h"

#include "chrip.h"

struct sonarData{
    QList<float> distance;
    QList<float> signal;
};

class SONAR
{
public:
    SONAR();
    QList<float> getDistance();
    void startSound();
    sonarData getResults();

    static const int air_speed = 343; // m/s
    static const int max_distance = 4; // meters


    int sample_desired_length;
    double max_time; // max time duration which take sound to go to and back

    static const bool distanceCompensation = true;

    static const bool distanceMediumRemoval = true;
    static const int distanceInitialisations = 10; // too long name

    bool distanceInitialisationDone = false;

    int distanceInitialisationRemaining = distanceInitialisations;
private:
    AudioPlayer outputPlayer;
    AudioRecorder inputPlayer;
    Chrip chrip;


    QList<float> sound;

    // distance related functions
    QList<float> correlateToDistance();
    int getStart(QList<float> data);
    QList<float> getDistance(QList<float> data);
    QList<float> distanceThresholdList;

private slots:
    void startRecording();
};

#endif // SONAR_H
