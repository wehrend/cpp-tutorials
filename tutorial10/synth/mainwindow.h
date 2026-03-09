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

// Definition der Wellenformen für bessere Lesbarkeit
enum WaveType {
    SINE = 0,
    SQUARE = 1,
    SAW = 2
};

class AudioGenerator : public QIODevice {
    Q_OBJECT
public:
    AudioGenerator(QObject *parent = nullptr) : QIODevice(parent) {}

    void setFrequency(float f) { m_frequency = f; }
    void setVolume(float v) { m_volume = v; }
    void setWaveType(int type) { m_waveType = type; }

    qint64 readData(char *data, qint64 maxlen) override {
        qint16 *samples = reinterpret_cast<qint16*>(data);
        int sampleCount = maxlen / sizeof(qint16);

        for (int i = 0; i < sampleCount; ++i) {
            float sample = 0;

            // Mathematische Generierung der Wellenformen
            if (m_waveType == SINE) {
                sample = qSin(m_phase);
            }
            else if (m_waveType == SQUARE) {
                sample = (qSin(m_phase) >= 0) ? 1.0f : -1.0f;
            }
            else if (m_waveType == SAW) {
                // Sägezahn: von -1.0 bis 1.0
                sample = (m_phase / M_PI) - 1.0f;
            }

            // Anwendung der Lautstärke und Konvertierung in 16-Bit PCM
            samples[i] = static_cast<qint16>(sample * 32767.0f * m_volume);

            m_phase += 2.0 * M_PI * m_frequency / 44100.0;
            if (m_phase > 2.0 * M_PI) m_phase -= 2.0 * M_PI;
        }
        return maxlen;
    }

    qint64 writeData(const char *data, qint64 len) override {
        (void)data; (void)len; return 0;
    }

private:
    float m_frequency = 440.0;
    float m_volume = 0.5f;
    int m_waveType = SINE;
    float m_phase = 0.0;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    QVector<QPointF> waveformPoints;
    float currentFreq = 440.0;
    int currentWaveType = SINE;

    void generateWaveform();

    AudioGenerator *m_generator;
    QAudioOutput *m_audioOutput;
};

#endif
