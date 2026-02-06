#ifndef ENVELOPE_H
#define ENVELOPE_H

enum EnvelopeState { OFF, ATTACK, DECAY, SUSTAIN, RELEASE };

class Envelope {
private:
    double amplitude = 0.0;
    double attackTime = 0.05;  // 50ms fade in
    double decayTime = 0.1;   // 100ms settle
    double sustainLevel = 0.7; // Hold at 70% volume
    double releaseTime = 0.3;  // 300ms fade out
    double sampleRate;
    EnvelopeState state = OFF;

public:
    Envelope(double sr) : sampleRate(sr) {}

    void triggerOn() { state = ATTACK; }
    void triggerOff() { state = RELEASE; }

    float getNextAmplitude() {
        switch (state) {
            case ATTACK:
                amplitude += 1.0 / (attackTime * sampleRate);
                if (amplitude >= 1.0) {
                    amplitude = 1.0;
                    state = DECAY;
                }
                break;
            case DECAY:
                amplitude -= (1.0 - sustainLevel) / (decayTime * sampleRate);
                if (amplitude <= sustainLevel) {
                    amplitude = sustainLevel;
                    state = SUSTAIN;
                }
                break;
            case SUSTAIN:
                amplitude = sustainLevel;
                break;
            case RELEASE:
                amplitude -= sustainLevel / (releaseTime * sampleRate);
                if (amplitude <= 0.0) {
                    amplitude = 0.0;
                    state = OFF;
                }
                break;
            case OFF:
                amplitude = 0.0;
                break;
        }
        return (float)amplitude;
    }

    EnvelopeState getState() const { return state; }
};

#endif