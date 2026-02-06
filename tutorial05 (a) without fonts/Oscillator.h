#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <cmath>

class Oscillator {
protected:
    double frequency, amplitude, phase, sampleRate;

public:
    // Nur Deklarationen (Semikolon statt geschweifter Klammern)
    Oscillator(double freq, double amp, double sr);
    virtual ~Oscillator() {}

    virtual float getNextSample() = 0; 
    void setFrequency(double freq);
};

// Unterklassen können hier im Header bleiben, wenn sie kurz sind
class SquareOsc : public Oscillator {
public:
    using Oscillator::Oscillator;
    float getNextSample() override; // Definition kommt in die .cpp
};

class SawOsc : public Oscillator {
public:
    using Oscillator::Oscillator;
    float getNextSample() override; // Definition kommt in die .cpp
};

class SineOsc : public Oscillator {
public:
    using Oscillator::Oscillator;
    float getNextSample() override; // Definition kommt in die .cpp
};

class TriangleOsc : public Oscillator {
public:
    using Oscillator::Oscillator;
    float getNextSample() override; // Definition kommt in die .cpp
};

#endif