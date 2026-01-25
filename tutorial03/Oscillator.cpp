#include "Oscillator.h"

Oscillator::Oscillator(double freq, double amp, double sr) 
    : frequency(freq), amplitude(amp), sampleRate(sr), phase(0.0) {}

float Oscillator::getNextSample() {
    // Hier nutzen wir jetzt die Square-Wave Logik
    float sample = (sin(phase) > 0) ? 1.0f : -1.0f;
    phase += (2.0 * M_PI * frequency) / sampleRate;
    
    if (phase > 2.0 * M_PI) phase -= 2.0 * M_PI;
    
    return sample * (float)amplitude;
}

void Oscillator::setFrequency(double freq) {
    frequency = freq;
}