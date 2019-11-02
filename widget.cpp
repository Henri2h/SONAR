/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "widget.h"

#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>

#include <QtWidgets/QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

#include <QtWidgets/QVBoxLayout>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

Widget::Widget(const QAudioDeviceInfo &deviceInfo, QWidget *parent) : QWidget(parent),
                                                                      m_chart_signal(new QChart),
                                                                      m_series_signal(new QLineSeries),
                                                                      m_chart_distance(new QChart),
                                                                      m_series_distance(new QLineSeries)
{

    // filling buffer
    int sampleCount = sn.sample_desired_length;

    if (m_buffer_signal.isEmpty())
    {
        m_buffer_signal.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i){
            qreal time_i = i * sn.max_time / sampleCount;
            m_buffer_signal.append(QPointF(time_i, 0.0));
        }
    }

    if (m_buffer_distance.isEmpty())
    {
        m_buffer_distance.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i){
            qreal distance_i = i * sn.max_distance * 1.0 / sampleCount;
            m_buffer_distance.append(QPointF(distance_i, 0));
        }
    }


    // UI : main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QChartView *chartView_signal = new QChartView(m_chart_signal);
    chartView_signal->setMinimumSize(1000, 400);
    m_chart_signal->addSeries(m_series_signal);

    QValueAxis *axisX_signal = new QValueAxis;
    axisX_signal->setRange(0, sn.max_time);
    axisX_signal->setLabelFormat("%g");
    axisX_signal->setTitleText("Samples");

    QValueAxis *axisY_signal = new QValueAxis;
    axisY_signal->setRange(-1, 1);
    axisY_signal->setTitleText("Audio level");
    m_chart_signal->addAxis(axisX_signal, Qt::AlignBottom);
    m_series_signal->attachAxis(axisX_signal);
    m_chart_signal->addAxis(axisY_signal, Qt::AlignLeft);
    m_series_signal->attachAxis(axisY_signal);
    m_chart_signal->legend()->hide();
    m_chart_signal->setTitle("Data from the microphone (" + deviceInfo.deviceName() + ')');

    mainLayout->addWidget(chartView_signal);


    QChartView *chartView_distance = new QChartView(m_chart_distance);

    chartView_distance->setMinimumSize(1000, 400);
    m_chart_distance->addSeries(m_series_distance);

    QValueAxis *axisX_distance = new QValueAxis;
    axisX_distance->setRange(0, sn.max_distance);
    axisX_distance->setLabelFormat("%g");
    axisX_distance->setTitleText("Distance (m)");

    QValueAxis *axisY_distance = new QValueAxis;
    axisY_distance->setRange(-1, 120);
    axisY_distance->setTitleText("Amplitude");

    m_chart_distance->addAxis(axisX_distance, Qt::AlignBottom);
    m_series_distance->attachAxis(axisX_distance);
    m_chart_distance->addAxis(axisY_distance, Qt::AlignLeft);
    m_series_distance->attachAxis(axisY_distance);
    m_chart_distance->legend()->hide();

    m_chart_signal->setTitle("Distance :");

    mainLayout->addWidget(chartView_distance);

    QLabel *qb = new QLabel(this);
    qb->setText("Hello");
    mainLayout->addWidget(qb);

    qb->setText("Start");

    startGettingDistance();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::startGettingDistance));
    timer->start(1300);
}
void addChart(){

}
void Widget::startGettingDistance()
{
    QTimer::singleShot(900, this, SLOT(getresults()));
    sn.startSound();
}
void Widget::DisplayPoints(QList<float> &data, QVector<QPointF> &buffer, QLineSeries &series){
    int l = data.length();
    if (l > buffer.length())
        l = buffer.length();
    for (int i = 0; i < l; i++)
    {
        buffer[i].setY(data[i]);
    }

    series.replace(buffer);
}
void Widget::getresults()
{
    sonarData snData = sn.getResults();
    DisplayPoints(snData.signal, m_buffer_signal, *m_series_signal);
    DisplayPoints(snData.distance, m_buffer_distance, *m_series_distance);
}

Widget::~Widget()
{
}
