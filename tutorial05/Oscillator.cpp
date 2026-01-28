#include "Oscillator.h"

Oscillator::Oscillator(double freq, double amp, double sr) 
    : frequency(freq), amplitude(amp), phase(0.0), sampleRate(sr) {}

void Oscillator::setFrequency(double freq) {
    frequency = freq;
}

float SquareOsc::getNextSample() {
    float sample = (sin(phase) > 0) ? 1.0f : -1.0f;
    phase += (2.0 * M_PI * frequency) / sampleRate;
    if (phase > 2.0 * M_PI) phase -= 2.0 * M_PI;
    return sample * (float)amplitude;
}

float SawOsc::getNextSample() {
    float sample = (float)(2.0 * (phase / (2.0 * M_PI)) - 1.0);
    phase += (2.0 * M_PI * frequency) / sampleRate;
    if (phase > 2.0 * M_PI) phase -= 2.0 * M_PI;
    return sample * (float)amplitude;
}

float SineOsc::getNextSample() {
    float sample = (float)sin(phase);
    phase += (2.0 * M_PI * frequency) / sampleRate;
    if (phase > 2.0 * M_PI) phase -= 2.0 * M_PI;
    return sample * (float)amplitude;
}

float TriangleOsc::getNextSample() {
    float sample = (float)((2.0 / M_PI) * asin(sin(phase)));
    phase += (2.0 * M_PI * frequency) / sampleRate;
    if (phase > 2.0 * M_PI) phase -= 2.0 * M_PI;
    return sample * (float)amplitude;
}