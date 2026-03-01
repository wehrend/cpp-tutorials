#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <cmath>

class Oscillator {
protected:
    float phase;
    float phaseIncrement;
    float frequency;
    float amplitude;
    float sampleRate;

public:
    Oscillator(float f, float a, float sr) : frequency(f), amplitude(a), sampleRate(sr), phase(0.0f) {
        updateIncrement();
    }

    virtual ~Oscillator() {}

    float getFrequency() const { return frequency; }

    // WICHTIG FÜR HARD SYNC: Phase manuell auslesen und setzen
    float getPhase() const { return phase; }
    void setPhase(float p) { phase = std::fmod(p, 1.0f); }

    void setFrequency(float f) {
        frequency = f;
        updateIncrement();
    }

    // Trennt den Fortschritt der Zeit von der Berechnung des Wertes
    void updatePhase() {
        phase += phaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;
    }

    // Jede Wellenform berechnet hier nur ihren Wert basierend auf 'phase'
    virtual float getSampleAtPhase(float p) = 0;

    // Standard-Methode für normalen Betrieb
    virtual float getNextSample() {
        float sample = getSampleAtPhase(phase);
        updatePhase();
        return sample;
    }

protected:
    void updateIncrement() {
        phaseIncrement = frequency / sampleRate;
    }
};

// Beispiel: SawOsc angepasst
class SawOsc : public Oscillator {
public:
    using Oscillator::Oscillator;
    float getSampleAtPhase(float p) override {
        return amplitude * (2.0f * p - 1.0f);
    }
};

// Beispiel: SquareOsc angepasst
class SquareOsc : public Oscillator {
public:
    using Oscillator::Oscillator;
    float getSampleAtPhase(float p) override {
        return (p < 0.5f) ? amplitude : -amplitude;
    }
};

#endif