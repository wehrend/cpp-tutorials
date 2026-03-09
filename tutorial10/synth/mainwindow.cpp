#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Audio-Format Setup
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    m_generator = new AudioGenerator(this);
    m_generator->open(QIODevice::ReadOnly);

    m_audioOutput = new QAudioOutput(format, this);
    m_audioOutput->start(m_generator);

    // SLIDER: Frequenz
    connect(ui->freqSlider, &QSlider::valueChanged, this, [this](int value) {
        currentFreq = static_cast<float>(value);
        m_generator->setFrequency(currentFreq);
        generateWaveform();
        update();
    });

    // SLIDER: Lautstärke (0-100 auf 0.0-1.0)
    connect(ui->volSlider, &QSlider::valueChanged, this, [this](int value) {
        m_generator->setVolume(value / 100.0f);
    });

    // DIAL: Wellenform (0=Sinus, 1=Rechteck, 2=Sägezahn)
    connect(ui->waveDial, &QDial::valueChanged, this, [this](int value) {
        currentWaveType = value;
        m_generator->setWaveType(value);
        generateWaveform();
        update();
    });

    generateWaveform();
}

void MainWindow::generateWaveform() {
    waveformPoints.clear();
    int w = width();
    for (int x = 0; x < w; ++x) {
        float timePhase = 2 * 3.14159 * currentFreq * (float)x / 10000.0;
        float y = 0;

        if (currentWaveType == SINE) {
            y = qSin(timePhase);
        } else if (currentWaveType == SQUARE) {
            y = (qSin(timePhase) >= 0) ? 1.0f : -1.0f;
        } else if (currentWaveType == SAW) {
            // Visuelle Annäherung für den Sägezahn
            y = fmod(timePhase, 2.0 * 3.14159) / 3.14159 - 1.0;
        }

        waveformPoints.append(QPointF(x, 200 + (y * 80)));
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Oszilloskop Hintergrund
    painter.fillRect(0, 100, width(), 200, Qt::black);

    // Grüne Linie
    painter.setPen(QPen(Qt::green, 2));
    painter.drawPolyline(waveformPoints.data(), waveformPoints.size());
}

MainWindow::~MainWindow() {
    m_audioOutput->stop();
    delete ui;
}
