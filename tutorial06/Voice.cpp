#include "Voice.h"

Voice::Voice(float freq, float amp, float sampleRate, int midiNote) 
    : env(sampleRate), note(midiNote), active(true) {
    
    // You can even make this switchable later!
    osc = new SawOsc(freq, amp, sampleRate); 
    env.triggerOn();
}

Voice::~Voice() {
    delete osc;
}

float Voice::getNextSample() {
    if (!osc) return 0.0f;
    return osc->getNextSample() * env.getNextAmplitude();
}