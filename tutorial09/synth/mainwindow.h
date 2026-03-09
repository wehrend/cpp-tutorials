#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPointF>
#include <QIODevice>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QAudioOutput;
class QPaintEvent;

class AudioGenerator : public QIODevice
{
    Q_OBJECT

public:
    explicit AudioGenerator(QObject *parent = nullptr);

    void setFrequency(float frequency);
    void setAmplitude(float amplitude);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    float m_frequency = 440.0f;
    float m_phase = 0.0f;
    float m_amplitude = 8000.0f;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static float sliderToLogFrequency(int value, int minValue, int maxValue,
                                      float minFreq, float maxFreq);
    static float sliderToLogAmplitude(int value, int minValue, int maxValue,
                                      float minAmp, float maxAmp);
    void generateWaveform();

    Ui::MainWindow *ui = nullptr;
    AudioGenerator *m_generator = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QVector<QPointF> waveformPoints;
    float currentFreq = 440.0f;
};

#endif // MAINWINDOW_H
