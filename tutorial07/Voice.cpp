#include "Voice.h"

Voice::Voice(float freq, float amp, float sampleRate, int midiNote) 
    : env(sampleRate), note(midiNote), active(true) {
    
    masterOsc = new SawOsc(freq, amp, sampleRate); 
    slaveOsc = new SawOsc(freq * 1.5f, amp, sampleRate); 
    env.triggerOn();
}

Voice::~Voice() {
    delete masterOsc;
    delete slaveOsc;
}

float Voice::getNextSample() {
    if (!masterOsc || !slaveOsc) return 0.0f;

    float oldPhase = masterOsc->getPhase();
    masterOsc->updatePhase();
    float newPhase = masterOsc->getPhase();

    if (newPhase < oldPhase) {
        slaveOsc->setPhase(0.0f); 
    }

    return slaveOsc->getNextSample() * env.getNextAmplitude();
}

void Voice::setSlaveFrequency(float freq) {
    if (slaveOsc) slaveOsc->setFrequency(freq);
}