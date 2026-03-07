#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Audio-Format definieren (Standard CD-Qualität)
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    // 2. Audio-Generator (Logik im Header) und Output initialisieren
    m_generator = new AudioGenerator(this);
    m_generator->open(QIODevice::ReadOnly);

    m_audioOutput = new QAudioOutput(format, this);
    m_audioOutput->start(m_generator);

    // 3. Slider-Verbindung: Ändert Tonhöhe und Grafik gleichzeitig
    connect(ui->freqSlider, &QSlider::valueChanged, this, [this](int value) {
        currentFreq = static_cast<float>(value);
        m_generator->setFrequency(currentFreq); // Ändert die Audio-Frequenz
        generateWaveform();                     // Berechnet neue Punkte für das Oszilloskop
        update();                               // Erzwingt das Neuzeichnen (paintEvent)
    });

    // Initiale Berechnung der Wellenform
    generateWaveform();
}

void MainWindow::generateWaveform() {
    waveformPoints.clear();
    // Wir nutzen die Breite des Fensters als Basis für die X-Achse
    for (int x = 0; x < width(); ++x) {
        // Die Konstante 3.14... ersetzt M_PI, falls Makros nicht gefunden werden
        float y = 80 * qSin(2 * 3.14159265 * currentFreq * (float)x / 10000.0);
        waveformPoints.append(QPointF(x, 200 + y));
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    (void)event; // Verhindert Warnung über ungenutzten Parameter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Zeichne den schwarzen "Bildschirm" des Oszilloskops
    painter.fillRect(0, 100, width(), 200, Qt::black);

    // Zeichne die grüne "Phosphor"-Linie
    painter.setPen(QPen(Qt::green, 2));
    painter.drawPolyline(waveformPoints.data(), waveformPoints.size());
}

MainWindow::~MainWindow() {
    m_audioOutput->stop(); // Audio-Stream sauber beenden
    delete ui;
}
