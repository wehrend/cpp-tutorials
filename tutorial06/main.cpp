#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <cmath>

#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"
#include "Voice.h"

// Shared buffer for the oscilloscope
std::vector<float> visualBuffer(2048, 0.0f);

class SynthEngine {
public:
    std::vector<Voice*> voices;
    LowPassFilter filter;

    SynthEngine() {
        filter.setCutoff(0.5f);
    }

    ~SynthEngine() {
        for (auto v : voices) delete v;
        voices.clear();
    }

    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            float mixedSample = 0.0f;

            // Iterate through active voices and sum them
            for (auto it = engine->voices.begin(); it != engine->voices.end(); ) {
                Voice* v = *it;
                
                // Get sample from voice (Osc * Env)
                float s = v->osc->getNextSample() * v->env.getNextAmplitude();
                mixedSample += s;

                // Lifecycle Management: If Envelope is OFF, kill the voice
                if (v->env.getState() == OFF) {
                    delete v;
                    it = engine->voices.erase(it);
                } else {
                    ++it;
                }
            }

            // Apply Global Filter and Master Volume (Headroom for chords)
            buffer[i] = engine->filter.process(mixedSample) * 0.3f;

            if (i < (int)visualBuffer.size()) {
                visualBuffer[i] = buffer[i];
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("C++ PolySynth v1.0", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Map computer keys to MIDI notes (60 = Middle C)
    std::map<SDL_Keycode, int> keyMap = {
        {SDLK_a, 60}, {SDLK_s, 62}, {SDLK_d, 64}, {SDLK_f, 65},
        {SDLK_g, 67}, {SDLK_h, 69}, {SDLK_j, 71}, {SDLK_k, 72}
    };

    SynthEngine engine;
    float currentCutoff = 0.5f;

    SDL_AudioSpec ds;
    ds.freq = 44100;
    ds.format = AUDIO_F32SYS;
    ds.channels = 1;
    ds.samples = 2048;
    ds.callback = SynthEngine::AudioCallback;
    ds.userdata = &engine;

    if (SDL_OpenAudio(&ds, NULL) < 0) return 1;
    SDL_PauseAudio(0);

    bool running = true;
    SDL_Event e;

    std::cout << "Polyphonic Synth Ready!\nKeys A-K: Play Notes\nUP/DOWN: Filter\nESC: Exit" << std::endl;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
                
                // Polyphonic Note On
                if (keyMap.count(e.key.keysym.sym)) {
                    int note = keyMap[e.key.keysym.sym];
                    float freq = 440.0f * pow(2.0f, (note - 69) / 12.0f);

                    SDL_LockAudio();
                    engine.voices.push_back(new Voice(freq, 0.2f, 44100.0f, note));
                    SDL_UnlockAudio();
                }

                if (e.key.keysym.sym == SDLK_UP) {
                    currentCutoff = std::min(1.0f, currentCutoff + 0.05f);
                    engine.filter.setCutoff(currentCutoff);
                }
                if (e.key.keysym.sym == SDLK_DOWN) {
                    currentCutoff = std::max(0.01f, currentCutoff - 0.05f);
                    engine.filter.setCutoff(currentCutoff);
                }
            }

            if (e.type == SDL_KEYUP) {
                // Polyphonic Note Off
                if (keyMap.count(e.key.keysym.sym)) {
                    int note = keyMap[e.key.keysym.sym];
                    
                    SDL_LockAudio();
                    for (auto v : engine.voices) {
                        if (v->note == note) v->env.triggerOff();
                    }
                    SDL_UnlockAudio();
                }
            }
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 15, 15, 20, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 50, 255, 200, 255);
        for (int x = 0; x < 799; x++) {
            int y1 = 200 - (int)(visualBuffer[x] * 150);
            int y2 = 200 - (int)(visualBuffer[x+1] * 150);
            SDL_RenderDrawLine(renderer, x, y1, x + 1, y2);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}