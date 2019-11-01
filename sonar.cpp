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
    float desired_length = format.sampleRate() * 2 * max_distance / (1.0*air_speed);
    sample_desired_length = desired_length; // to int truncature
}

QList<float> SONAR::getDistance(QList<float> data)
{
    QList<float> distances;

    if(data.length() < sound.length()){
        qWarning() << "Sound lenght should be smaller than data. Data lenght : " << data.length() << " Sound length :  " << sound.length();
        return distances;
    }
    float max = 0;

    for (int i = 0; i < data.length() - sound.length(); i++){

        // get max pos
        int max_pos =  sound.length();
        if(max_pos > data.length()-i) max_pos = data.length() - i;

        float distance = 0;
        for(int pos=0; pos < sound.length(); pos++ ){
            distance += abs(data[i+pos]*sound[pos]);
        }
        if(distance > max) max = distance;

        distance *= i/1000.0;
        distances.append(distance);
    }
    qDebug() << "Max : " << max;
    return distances;
}

int SONAR::getStart(QList<float> data){
    const float a = 0.8; // coef
    const float threshold = 0.4;//
    const int delta = 50; // since we detect the singla while the signal has already risen (at the max) we have to go back at the begin of the perturbation
    float s = 0;

    for(int i =0; i < data.length(); i++){
        s = data[i]*a + s*(1-a);
        //qDebug() << s;
        if(s > threshold){

            if(i > delta){
            return i-delta;
            }
            else{
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

QList<float> SONAR::getResults()
{
    auto data = inputPlayer.getRecording();


    // detect the start and remove first elements
    /*
    We do this to have allways the same array, begin at the same time
    We truncate the end to have always the same lenght
*/
    int start = getStart(data);
    for (int i = 0; i < start; i++){
        data.removeFirst();
    }
    for (int i = start+sample_desired_length; i < data.length(); i++){
        data.removeLast();
    }

    QList<float> distance = getDistance(data);

    // test if playback work
    //outputPlayer.Play(data);

    return distance;
}
