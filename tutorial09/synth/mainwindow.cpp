#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAudioFormat>
#include <QAudioOutput>
#include <QPainter>
#include <QPaintEvent>
#include <QSlider>
#include <QtMath>

namespace {
constexpr float kMinFreq = 20.0f;
constexpr float kMaxFreq = 20000.0f;
constexpr float kMinAmp = 0.0f;
constexpr float kMaxAmp = 16000.0f;
constexpr int kFreqSliderDefault = 895; // ~440 Hz on a 0..2000 logarithmic slider
}

AudioGenerator::AudioGenerator(QObject *parent)
    : QIODevice(parent)
{
}

void AudioGenerator::setFrequency(float frequency)
{
    m_frequency = frequency;
}

void AudioGenerator::setAmplitude(float amplitude)
{
    m_amplitude = amplitude;
}

qint64 AudioGenerator::readData(char *data, qint64 maxlen)
{
    qint16 *samples = reinterpret_cast<qint16 *>(data);
    const int sampleCount = static_cast<int>(maxlen / static_cast<qint64>(sizeof(qint16)));

    for (int i = 0; i < sampleCount; ++i) {
        const float value = m_amplitude * qSin(m_phase);
        samples[i] = static_cast<qint16>(value);

        m_phase += static_cast<float>(2.0 * M_PI) * m_frequency / 44100.0f;
        if (m_phase >= static_cast<float>(2.0 * M_PI)) {
            m_phase -= static_cast<float>(2.0 * M_PI);
        }
    }

    return static_cast<qint64>(sampleCount) * static_cast<qint64>(sizeof(qint16));
}

qint64 AudioGenerator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
    return 0;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_generator(new AudioGenerator(this))
{
    ui->setupUi(this);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    m_generator->open(QIODevice::ReadOnly);
    m_generator->setAmplitude(8000.0f);

    m_audioOutput = new QAudioOutput(format, this);
    m_audioOutput->start(m_generator);

    ui->freqSlider->setRange(0, 2000);

    connect(ui->freqSlider, &QSlider::valueChanged, this, [this](int value) {
        currentFreq = sliderToLogFrequency(value,
                                           ui->freqSlider->minimum(),
                                           ui->freqSlider->maximum(),
                                           kMinFreq,
                                           kMaxFreq);
        m_generator->setFrequency(currentFreq);
        generateWaveform();
        update();
    });

    if (QSlider *volumeSlider = findChild<QSlider *>(QStringLiteral("volumeSlider"))) {
        connect(volumeSlider, &QSlider::valueChanged, this, [this, volumeSlider](int value) {
            const float amplitude = sliderToLogAmplitude(value,
                                                         volumeSlider->minimum(),
                                                         volumeSlider->maximum(),
                                                         kMinAmp,
                                                         kMaxAmp);
            m_generator->setAmplitude(amplitude);
        });
    }

    ui->freqSlider->setValue(kFreqSliderDefault);
    currentFreq = sliderToLogFrequency(ui->freqSlider->value(),
                                       ui->freqSlider->minimum(),
                                       ui->freqSlider->maximum(),
                                       kMinFreq,
                                       kMaxFreq);
    m_generator->setFrequency(currentFreq);
    generateWaveform();
}

MainWindow::~MainWindow()
{
    if (m_audioOutput != nullptr) {
        m_audioOutput->stop();
    }
    delete ui;
}

float MainWindow::sliderToLogFrequency(int value, int minValue, int maxValue,
                                       float minFreq, float maxFreq)
{
    if (maxValue <= minValue || minFreq <= 0.0f || maxFreq <= minFreq) {
        return minFreq;
    }

    const float normalized = static_cast<float>(value - minValue)
                           / static_cast<float>(maxValue - minValue);
    return minFreq * qPow(maxFreq / minFreq, normalized);
}

float MainWindow::sliderToLogAmplitude(int value, int minValue, int maxValue,
                                       float minAmp, float maxAmp)
{
    if (maxValue <= minValue || maxAmp < minAmp) {
        return minAmp;
    }

    const float normalized = static_cast<float>(value - minValue)
                           / static_cast<float>(maxValue - minValue);

    if (normalized <= 0.0f) {
        return minAmp;
    }

    return minAmp + (maxAmp - minAmp) * qPow(normalized, 2.0f);
}

void MainWindow::generateWaveform()
{
    waveformPoints.clear();
    waveformPoints.reserve(width());

    for (int x = 0; x < width(); ++x) {
        const float y = 80.0f * qSin((2.0f * static_cast<float>(M_PI) * currentFreq * static_cast<float>(x)) / 10000.0f);
        waveformPoints.append(QPointF(x, 200.0 + y));
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0, 100, width(), 200, Qt::black);
    painter.setPen(QPen(Qt::green, 2));

    if (!waveformPoints.isEmpty()) {
        painter.drawPolyline(waveformPoints.constData(), waveformPoints.size());
    }
}
