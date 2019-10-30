#ifndef CHRIP_H
#define CHRIP_H

#include <QFile>
#include <QStringList>
#include <QDebug>

class Chrip
{
public:
    Chrip();

    QList<float> getChrip();
    QList<float> getTime();

private:
    void loadChrip();

    QList<float> time;
    QList<float> amplitude;
};

#endif // CHRIP_H
