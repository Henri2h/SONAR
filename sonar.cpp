#include "sonar.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

SONAR::SONAR()
{
    qDebug() << "Sonar started";
  //  outputPlayer.startPlaying();

    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    inputPlayer.initializeAudio(format);
    outputPlayer.initializeAudio(format);
}

void SONAR::getDistance(){
}


void SONAR::startSound(){
    auto sound =  chrip.getChrip();
    inputPlayer.Record(2);
    outputPlayer.Play(sound);
}

QList<float> SONAR::getResults(){
    auto data = inputPlayer.getRecording();
    qDebug() << "Going to play : ";
    outputPlayer.Play(data);
    return data;
}


