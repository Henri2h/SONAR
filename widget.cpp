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
                                                                      m_chart(new QChart),
                                                                      m_series(new QLineSeries)
{

    // load buffer
    int sampleCount = 20000;

    if (m_buffer.isEmpty())
    {
        m_buffer.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i)
            m_buffer.append(QPointF(i, 0));
    }

    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(800, 600);
    m_chart->addSeries(m_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, sampleCount);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-1, 120);
    axisY->setTitleText("Audio level");
    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_series->attachAxis(axisX);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisY);
    m_chart->legend()->hide();
    m_chart->setTitle("Data from the microphone (" + deviceInfo.deviceName() + ')');

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);

    QLabel *qb = new QLabel(this);
    qb->setText("Hello");
    mainLayout->addWidget(qb);

    qb->setText("Start");

    startGettingDistance();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::startGettingDistance));
    timer->start(1300);
}

void Widget::startGettingDistance()
{
    QTimer::singleShot(900, this, SLOT(getresults()));
    sn.startSound();
}

void Widget::getresults()
{
    QList<float> data = sn.getResults();
    int l = data.length();
    if (l > m_buffer.length())
        l = m_buffer.length();
    for (int i = 0; i < l; i++)
    {
        m_buffer[i].setY(data[i]);
    }

    m_series->replace(m_buffer);
}

Widget::~Widget()
{
}
