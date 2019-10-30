#include "chrip.h"


Chrip::Chrip()
{
    // load chrip in memory
    loadChrip();
}

void Chrip::loadChrip()
{
    QFile file("Data/Chirp.csv");
       if (!file.open(QIODevice::ReadOnly)) {
           qDebug() << file.errorString();
           return;
       }

       file.readLine(); // remove first column

       while (!file.atEnd()) {
           QByteArray line = file.readLine();
           QList<QByteArray> strings = line.split(',');

           time.append(strings[0].toFloat());
           amplitude.append(strings[1].toFloat());

        //   qDebug() << "Word list : |" << strings[0].toFloat() << "| : |" << strings[1].toFloat() << "|";
       }

}

QList<float> Chrip::getChrip(){
    return amplitude;
}

QList<float> Chrip::getTime(){
    return time;
}
