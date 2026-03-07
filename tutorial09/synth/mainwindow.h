#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPointF>
#include <QtMath>
#include <QAudioOutput>
#include <QIODevice>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    // In der mainwindow.h unter private:
    class AudioGenerator *m_generator;
    class QAudioOutput *m_audioOutput;

protected:
    void paintEvent(QPaintEvent *event) override; // This draws the waveform

private:
    QVector<QPointF> waveformPoints;
    float currentFreq = 440.0;
    void generateWaveform(); // Logic to calculate the sine wave
};

class AudioGenerator : public QIODevice {
    Q_OBJECT
public:
    // Konstruktor mit Parent, damit Qt den Speicher verwalten kann
    AudioGenerator(QObject *parent = nullptr) : QIODevice(parent) {}

    void setFrequency(float f) { m_frequency = f; }

    qint64 readData(char *data, qint64 maxlen) override {
        qint16 *samples = reinterpret_cast<qint16*>(data);
        int sampleCount = maxlen / sizeof(qint16);
        for (int i = 0; i < sampleCount; ++i) {
            float val = 8000.0 * qSin(m_phase);
            samples[i] = static_cast<qint16>(val);
            m_phase += 2.0 * M_PI * m_frequency / 44100.0;
            if (m_phase > 2.0 * M_PI) m_phase -= 2.0 * M_PI;
        }
        return maxlen;
    }

    qint64 writeData(const char *data, qint64 len) override {
        Q_UNUSED(data); Q_UNUSED(len); return 0;
    }

private:
    float m_frequency = 440.0;
    float m_phase = 0.0;
};
#endif // MAINWINDOW_H
