#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtCharts/QChartGlobal>
#include <QtCharts>

#include "sonar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void startGettingDistance();
    void getresults();

private:

    // charts :
    QChart *m_chart_signal;
    QChart *m_chart_distance;
    QLineSeries *m_series_signal;
    QLineSeries *m_series_distance;


    SONAR sn;
    QTimer *timer;

    QVector<QPointF> m_buffer_signal;
    QVector<QPointF> m_buffer_distance;

    void DisplayPoints(QList<float> &data, QVector<QPointF> &buffer, QLineSeries &series);
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
