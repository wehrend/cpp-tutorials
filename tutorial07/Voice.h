#ifndef VOICE_H
#define VOICE_H

#include "Oscillator.h"
#include "Envelope.h"

class Voice {
public:
    Oscillator* masterOsc;
    Oscillator* slaveOsc;
    Envelope env;
    int note;        // To track which key is being held
    bool active;

    Voice(float freq, float amp, float sampleRate, int midiNote);
    ~Voice();

    float getNextSample();
    void setSlaveFrequency(float freq);
};

#endif