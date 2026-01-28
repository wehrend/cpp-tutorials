#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"

// Shared buffer for visualization
std::vector<float> visualBuffer(2048, 0.0f);

class SynthEngine {
public:
    Oscillator* osc;
    Envelope env;
    LowPassFilter filter;

    SynthEngine() : env(48000.0) {
        // Start with a Square Wave
        osc = new SquareOsc(440.0, 0.2, 48000.0);
        filter.setCutoff(0.05f);
    }

    ~SynthEngine() {
        delete osc;
    }

    // AUDIO CALLBACK: The Real-Time Heartbeat
    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            float sample = 0.0f;

            // 1. Generate raw sound (Inheritance/Polymorphism)
            if (engine->osc != nullptr) {
                sample = engine->osc->getNextSample();
            }

            // 2. Process through VCF (Composition)
            sample = engine->filter.process(sample);

            // 3. Shape volume with ADSR (Composition)
            float currentAmplitude = engine->env.getNextAmplitude();
            float finalSample = sample * currentAmplitude;

            buffer[i] = finalSample;

            // Update visualization buffer
            if (i < (int)visualBuffer.size()) {
                visualBuffer[i] = finalSample;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("C++ Synth: VCF & ADSR", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SynthEngine engine;
    float currentCutoff = 0.5f;

    SDL_AudioSpec ds;
    ds.freq = 44100;
    ds.format = AUDIO_F32SYS;
    ds.channels = 1;
    ds.samples = 2048;
    ds.callback = SynthEngine::AudioCallback;
    ds.userdata = &engine;

    SDL_OpenAudio(&ds, NULL);
    SDL_PauseAudio(0);

    std::cout << "Controls:\n1-4: Waveforms\nSPACE: Trigger ADSR\nUP/DOWN: Filter Cutoff\nESC: Exit" << std::endl;

    bool running = true;
    SDL_Event e;
    
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_KEYDOWN) {
                // Waveform Switching
                Oscillator* nextOsc = nullptr;
                switch (e.key.keysym.sym) {
                    case SDLK_1: nextOsc = new SquareOsc(440.0, 0.2, 44100.0); break;
                    case SDLK_2: nextOsc = new SawOsc(440.0, 0.2, 44100.0); break;
                    case SDLK_3: nextOsc = new SineOsc(440.0, 0.2, 44100.0); break;
                    case SDLK_4: nextOsc = new TriangleOsc(440.0, 0.2, 44100.0); break;
                    
                    // Filter Control
                    case SDLK_UP: 
                        currentCutoff = std::min(1.0f, currentCutoff + 0.05f);
                        engine.filter.setCutoff(currentCutoff);
                        break;
                    case SDLK_DOWN: 
                        currentCutoff = std::max(0.01f, currentCutoff - 0.05f);
                        engine.filter.setCutoff(currentCutoff);
                        break;
                        
                    case SDLK_ESCAPE: running = false; break;
                }

                if (nextOsc != nullptr) {
                    Oscillator* oldOsc = engine.osc;
                    engine.osc = nullptr; // Safety swap
                    delete oldOsc;
                    engine.osc = nextOsc;
                }
            }
        }

        // ADSR Triggering based on Spacebar
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_SPACE]) {
            if (engine.env.getState() == OFF || engine.env.getState() == RELEASE) {
                engine.env.triggerOn();
            }
        } else {
            if (engine.env.getState() != OFF && engine.env.getState() != RELEASE) {
                engine.env.triggerOff();
            }
        }

        // Rendering logic
        SDL_SetRenderDrawColor(renderer, 15, 15, 20, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 255, 128, 255); // Neon Green
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