#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Include your custom headers
#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"

// Shared buffer for the oscilloscope visualization
std::vector<float> visualBuffer(2048, 0.0f);

// --- THE SYNTH ENGINE ---
// Demonstrating COMPOSITION: The Engine "has" an Osc, an Envelope, and a Filter.
class SynthEngine {
public:
    Oscillator* osc;
    Envelope env;
    LowPassFilter filter;

    SynthEngine() : env(44100.0) {
        // Start with a Sawtooth wave for a rich harmonic sound
        osc = new SawOsc(440.0, 0.2, 44100.0);
        filter.setCutoff(0.5f);
    }

    ~SynthEngine() {
        delete osc;
    }

    // Real-time Audio Callback
    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            float sample = 0.0f;

            // 1. Generate Raw Signal
            if (engine->osc != nullptr) {
                sample = engine->osc->getNextSample();
            }

            // 2. Process through Filter (VCF)
            sample = engine->filter.process(sample);

            // 3. Apply Amplitude Envelope (ADSR)
            float currentAmp = engine->env.getNextAmplitude();
            float finalSample = sample * currentAmp;

            buffer[i] = finalSample;

            // Update visualizer
            if (i < (int)visualBuffer.size()) {
                visualBuffer[i] = finalSample;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    // 1. System Init
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) return 1;
    if (TTF_Init() < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("C++ PolySynth v1.0", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // 2. Resource Loading
    TTF_Font* font = TTF_OpenFont("arial.ttf", 22);
    if (!font) std::cerr << "Font missing! Place arial.ttf in the project folder." << std::endl;

    // 3. Engine State
    SynthEngine engine;
    float currentCutoff = 0.5f;
    std::string currentWaveName = "Sawtooth";

    // 4. Audio Config
    SDL_AudioSpec ds;
    ds.freq = 44100;
    ds.format = AUDIO_F32SYS;
    ds.channels = 1;
    ds.samples = 2048;
    ds.callback = SynthEngine::AudioCallback;
    ds.userdata = &engine;

    if (SDL_OpenAudio(&ds, NULL) < 0) return 1;
    SDL_PauseAudio(0);

    // 5. Main Loop
    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                Oscillator* nextOsc = nullptr;
                float f = 440.0, a = 0.2, r = 44100.0;

                switch (e.key.keysym.sym) {
                    case SDLK_1: nextOsc = new SineOsc(f, a, r);     currentWaveName = "Sine";     break;
                    case SDLK_2: nextOsc = new SquareOsc(f, a, r);   currentWaveName = "Square";   break;
                    case SDLK_3: nextOsc = new SawOsc(f, a, r);      currentWaveName = "Sawtooth"; break;
                    case SDLK_4: nextOsc = new TriangleOsc(f, a, r); currentWaveName = "Triangle"; break;
                    
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

                // The Pointer Swap (Polymorphism)
                if (nextOsc) {
                    Oscillator* old = engine.osc;
                    engine.osc = nextOsc; 
                    delete old;
                }
            }
        }

        // Spacebar for ADSR
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_SPACE]) engine.env.triggerOn();
        else engine.env.triggerOff();

        // 6. Rendering
        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_RenderClear(renderer);

        // Oscilloscope
        SDL_SetRenderDrawColor(renderer, 50, 255, 200, 255);
        for (int x = 0; x < 799; x++) {
            int y1 = 200 - (int)(visualBuffer[x] * 150);
            int y2 = 200 - (int)(visualBuffer[x+1] * 150);
            SDL_RenderDrawLine(renderer, x, y1, x + 1, y2);
        }

        // HUD Text
        if (font) {
            std::string uiText = "Wave: " + currentWaveName + " | Cutoff: " + std::to_string((int)(currentCutoff * 100)) + "%";
            SDL_Color color = { 255, 255, 255, 255 };
            SDL_Surface* s = TTF_RenderText_Blended(font, uiText.c_str(), color);
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            int tw, th;
            SDL_QueryTexture(t, NULL, NULL, &tw, &th);
            SDL_Rect dst = { 20, 20, tw, th };
            SDL_RenderCopy(renderer, t, NULL, &dst);
            SDL_FreeSurface(s);
            SDL_DestroyTexture(t);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 7. Cleanup
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}