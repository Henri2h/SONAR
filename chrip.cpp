#include "chrip.h"

Chrip::Chrip()
{
    // load chrip in memory
    loadChrip();
}

void Chrip::loadChrip()
{
    int numberOfPulse = 1;
    int spacingBetweenPulses = 30;

    QFile file("Data/Chirp.csv");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << file.errorString();
        return;
    }

    file.readLine(); // remove first column

    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        QList<QByteArray> strings = line.split(',');

        time.append(strings[0].toFloat());
        amplitude.append(strings[1].toFloat());

        //   qDebug() << "Word list : |" << strings[0].toFloat() << "| : |" << strings[1].toFloat() << "|";
    }

    int chirp_len = amplitude.length();
    // multiple pulses
    for(int i = 1; i < numberOfPulse; i ++){
        for(int j = 0; j < spacingBetweenPulses; j++){
            amplitude.append(0);
        }
        for(int j = 0; j < chirp_len; j++){
            amplitude.append(amplitude[j]);
        }
    }
}

QList<float> Chrip::getChrip()
{
    return amplitude;
}

QList<float> Chrip::getTime()
{
    return time;
}
