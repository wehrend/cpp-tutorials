#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <cmath>

class Oscillator {
private:
    double frequency;
    double amplitude;
    double phase;
    double sampleRate;

public:
    Oscillator(double freq, double amp, double sr);
    
    // Nur die Signatur der Methode
    float getNextSample();
    void setFrequency(double freq);
};

#endif