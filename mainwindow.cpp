#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lb_ready->setText("We are ready");

    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    int nx = 200;
    int ny = 200;
    colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(-4, 4), QCPRange(-4, 4)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    double x, y, z;
    for (int xIndex=0; xIndex<nx; ++xIndex)
    {
      for (int yIndex=0; yIndex<ny; ++yIndex)
      {
        colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
        double r = 3*qSqrt(x*x+y*y)+1e-2;
        z = 2*x*(qCos(r+2)/r-qSin(r+2)/r); // the B field strength of dipole radiation (modulo physical constants)
        colorMap->data()->setCell(xIndex, yIndex, z);
      }
    }

    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpPolar);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();


    // filling buffer
    int sampleCount = sn.sample_desired_length;

    if (m_buffer_signal.isEmpty())
    {
        m_buffer_signal.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i)
        {
            qreal time_i = i * sn.max_time / sampleCount;
            m_buffer_signal.append(QPointF(time_i, 0.0));
        }
    }

    if (m_buffer_distance.isEmpty())
    {
        m_buffer_distance.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i)
        {
            qreal distance_i = i * sn.max_distance * 1.0 / sampleCount;
            m_buffer_distance.append(QPointF(distance_i, 0));
        }
    }

    // UI : main layout
    m_chart_signal = new QChart();
    m_chart_distance = new QChart();

    m_series_signal = new QLineSeries();
    m_series_distance = new QLineSeries();

    // asign them
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
    m_chart_signal->setTitle("Data from the microphone");


    m_chart_distance->addSeries(m_series_distance);

    QValueAxis *axisX_distance = new QValueAxis;
    axisX_distance->setRange(0, sn.max_distance);
    axisX_distance->setLabelFormat("%g");
    axisX_distance->setTitleText("Distance (m)");

    QValueAxis *axisY_distance = new QValueAxis;
    axisY_distance->setRange(0, 60);
    axisY_distance->setTitleText("Amplitude");

    m_chart_distance->addAxis(axisX_distance, Qt::AlignBottom);
    m_series_distance->attachAxis(axisX_distance);
    m_chart_distance->addAxis(axisY_distance, Qt::AlignLeft);
    m_series_distance->attachAxis(axisY_distance);
    m_chart_distance->legend()->hide();

    m_chart_signal->setTitle("Distance :");

    // set charts
    ui->ChartSignal->setChart(m_chart_signal);
    ui->chartDistance->setChart(m_chart_distance);

    startGettingDistance();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::startGettingDistance));
    timer->start(1300);
}

void MainWindow::startGettingDistance(){
    QTimer::singleShot(900, this, SLOT(getresults()));
    sn.startSound();
}

void MainWindow::getresults(){
    sonarData snData = sn.getResults();
    DisplayPoints(snData.signal, m_buffer_signal, *m_series_signal);
    DisplayPoints(snData.distance, m_buffer_distance, *m_series_distance);

    if(sn.distanceInitialisationDone){
        ui->lb_ready->setText("Ready !");
    }
}

void MainWindow::DisplayPoints(QList<float> &data, QVector<QPointF> &buffer, QLineSeries &series)
{
    int l = data.length();
    if (l > buffer.length())
        l = buffer.length();
    for (int i = 0; i < l; i++)
    {
        buffer[i].setY(data[i]);
    }

    series.replace(buffer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

