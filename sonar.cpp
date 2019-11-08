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

    max_time = 2 * max_distance / (1.0 * air_speed);
    float desired_length = format.sampleRate() * max_time;
    sample_desired_length = desired_length; // to int truncature
}

QList<float> SONAR::getDistance(QList<float> data)
{
    QList<float> distances;

    if (data.length() < sound.length())
    {
        qWarning() << "Sound lenght should be smaller than data. Data lenght : " << data.length() << " Sound length :  " << sound.length();
        return distances;
    }
    float max = 0;

    for (int i = 0; i < data.length() - sound.length(); i++)
    {

        // get max pos
        int max_pos = sound.length();
        if (max_pos > data.length() - i)
            max_pos = data.length() - i;

        float distance = 0;
        for (int pos = 0; pos < sound.length(); pos++)
        {
            distance += abs(data[i + pos] * sound[pos]);
        }
        if (distance > max)
            max = distance;
        if (distanceCompensation)
            distance *= i / 1000.0;

        if (distanceMediumRemoval)
        {
            if (distanceInitialisationRemaining > 0)
            {
                float a = distance / distanceInitialisations;
                if (distanceThresholdList.length() - 1 < i) // does the i element exist ?
                    distanceThresholdList.append(a);
                else
                    distanceThresholdList[i] += a;
            }
            else
            {
                distance -= distanceThresholdList[i];
            }
        }


        distances.append(distance);
    }


    // distance medium removal control
    if (distanceMediumRemoval)
    {
        if (distanceInitialisationRemaining > 0)
        {

            qDebug() << "Distance initialisationRemainig : " << distanceInitialisationRemaining;
            distanceInitialisationRemaining--;
        }
        else
        {
            if (distanceInitialisationDone == false)
            {
                distanceInitialisationDone = true;
                qDebug() << "Distance initialisation done";
            }
        }
    }

    return distances;
}

int SONAR::getStart(QList<float> data)
{
    const float a = 0.8;         // coef
    const float threshold = 0.4; //
    const int delta = 50;        // since we detect the singla while the signal has already risen (at the max) we have to go back at the begin of the perturbation
    float s = 0;

    for (int i = 0; i < data.length(); i++)
    {
        s = data[i] * a + s * (1 - a);
        //qDebug() << s;
        if (s > threshold)
        {

            if (i > delta)
            {
                return i - delta;
            }
            else
            {
                return i;
            }
        }
    }

    return 0;
}

void SONAR::startSound()
{
    sound = chrip.getChrip();

    inputPlayer.Record(800); // 300 ms
    outputPlayer.Play(sound);
}

sonarData SONAR::getResults()
{
    sonarData snData;
    snData.signal = inputPlayer.getRecording();

    if(snData.signal.length() < sample_desired_length){ // invalid data, we fill until we have enough data
        for(int pos= snData.signal.length(); pos < sample_desired_length; pos++)
            snData.signal.append(0);
    }
    else{

        // we have valid data, we can analyse it
        // detect the start and remove first elements
        /*
        We do this to have allways the same array, begin at the same time
        We truncate the end to have always the same lenght
    */
        int start = getStart(snData.signal);
        for (int i = 0; i < start; i++)
        {
            snData.signal.removeFirst();
        }
        for (int i = start + sample_desired_length; i < snData.signal.length(); i++)
        {
            snData.signal.removeLast();
        }
    }
    snData.distance = getDistance(snData.signal);

    // test if playback work
    //outputPlayer.Play(data);

    return snData;
}
