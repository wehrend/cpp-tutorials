#ifndef FILTER_H
#define FILTER_H

class LowPassFilter {
private:
    float lastSample = 0.0f;
    float cutoff = 0.1f; // Range 0.0 to 1.0

public:
    // Simple smoothing formula: 
    // y[n] = y[n-1] + cutoff * (x[n] - y[n-1])
    float process(float input) {
        float output = lastSample + cutoff * (input - lastSample);
        lastSample = output;
        return output;
    }

    void setCutoff(float newCutoff) {
        if (newCutoff > 1.0f) cutoff = 1.0f;
        else if (newCutoff < 0.0f) cutoff = 0.0f;
        else cutoff = newCutoff;
    }
};

#endif