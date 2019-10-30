#include "sonar.h"

SONAR::SONAR()
{
    qDebug() << "Sonar started";

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

void SONAR::getDistance()
{
}

void SONAR::startSound()
{
    auto sound = chrip.getChrip();
    inputPlayer.Record(800); // 300 ms
    outputPlayer.Play(sound);
}

QList<float> SONAR::getResults()
{
    auto data = inputPlayer.getRecording();
    qDebug() << "Going to play : ";

    // test if playback work
    //outputPlayer.Play(data);
    return data;
}
