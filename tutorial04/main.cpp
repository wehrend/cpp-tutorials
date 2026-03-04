#ifdef _WIN32
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif
#include <iostream>
#include <vector>
#include "Oscillator.h"
#include "Envelope.h"

std::vector<float> visualBuffer(2048, 0.0f);

class SynthEngine {
public:
    Oscillator* osc;
    Envelope env; // Composition: Engine "has-a" Envelope

    SynthEngine() : env(48000.0) {
        osc = new SquareOsc(440.0, 0.2, 48000.0);
    }

    ~SynthEngine() { delete osc; }

    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            float rawSample = 0.0f;
            
            // Check for valid oscillator (thread safety)
            if (engine->osc != nullptr) {
                rawSample = engine->osc->getNextSample();
            }
            
            // Apply the envelope to the sound
            float currentVolume = engine->env.getNextAmplitude();
            float finalSample = rawSample * currentVolume;

            buffer[i] = finalSample;

            if (i < (int)visualBuffer.size()) {
                visualBuffer[i] = finalSample;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("C++ Synth: ADSR & OOP", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SynthEngine engine;

    SDL_AudioSpec ds;
    ds.freq = 44100;
    ds.format = AUDIO_F32SYS;
    ds.channels = 1;
    ds.samples = 2048;
    ds.callback = SynthEngine::AudioCallback;
    ds.userdata = &engine;

    SDL_OpenAudio(&ds, NULL);
    SDL_PauseAudio(0);

    bool running = true;
    SDL_Event e;
    
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            
            if (e.type == SDL_KEYDOWN) {
                Oscillator* nextOsc = nullptr;
                switch (e.key.keysym.sym) {
                    case SDLK_1: nextOsc = new SquareOsc(440.0, 0.2, 44100.0); break;
                    case SDLK_2: nextOsc = new SawOsc(440.0, 0.2, 44100.0); break;
                    case SDLK_3: nextOsc = new SineOsc(440.0, 0.2, 44100.0); break;
                    case SDLK_4: nextOsc = new TriangleOsc(440.0, 0.2, 44100.0); break;
                    case SDLK_ESCAPE: running = false; break;
                }

                if (nextOsc != nullptr) {
                    Oscillator* oldOsc = engine.osc;
                    engine.osc = nullptr;
                    delete oldOsc;
                    engine.osc = nextOsc;
                }
            }
        }

        // --- ENVELOPE TRIGGERING ---
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_SPACE]) {
            // Trigger Attack if we are currently silent or releasing
            if (engine.env.getState() == OFF || engine.env.getState() == RELEASE) {
                engine.env.triggerOn();
            }
        } else {
            // Trigger Release if we are currently holding a note
            if (engine.env.getState() != OFF && engine.env.getState() != RELEASE) {
                engine.env.triggerOff();
            }
        }

        // --- RENDERING ---
        SDL_SetRenderDrawColor(renderer, 10, 10, 15, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 255, 150, 255);
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