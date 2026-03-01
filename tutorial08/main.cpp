#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"
#include "Voice.h"
#include "AudioRingBuffer.h"

// Global pre-allocated Ring Buffer for the Oscilloscope (2048 samples)
AudioRingBuffer<float, 2048> scopeBuffer;

class SynthEngine {
public:
    // std::vector is great for iterating through active voices in the audio thread.
    // However, adding/removing voices requires a Mutex (SDL_LockAudio) to be safe.
    std::vector<Voice*> voices;
    LowPassFilter filter;
    SquareOsc lfo;
    
    float masterVolume = 0.15f;
    float syncMultiplier = 2.5f; 

    SynthEngine() : lfo(0.5f, 1.0f, 44100.0f) {
        filter.setCutoff(0.8f);
    }

    ~SynthEngine() {
        for (auto v : voices) delete v;
    }

    // This is the High-Priority Audio Thread
    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            float mixedSample = 0.0f;

            // Process all active voices stored in the std::vector
            for (auto it = engine->voices.begin(); it != engine->voices.end(); ) {
                Voice* v = *it;
                
                // Real-time update of the Hard Sync frequency
                v->setSlaveFrequency(v->masterOsc->getFrequency() * engine->syncMultiplier);

                mixedSample += v->getNextSample();

                // If Envelope is finished, remove the voice from the vector
                if (v->env.getState() == OFF) {
                    delete v;
                    it = engine->voices.erase(it);
                } else {
                    ++it;
                }
            }

            // Global FX Chain
            float filtered = engine->filter.process(mixedSample);
            float finalSample = filtered * engine->masterVolume;
            
            buffer[i] = finalSample;

            // PUSH to the custom lock-free structure for the UI to see
            scopeBuffer.write(finalSample);
        }
    }
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("C++ Hard Sync Synth", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // std::map: Perfect for translating keys to MIDI notes at the UI level
    std::map<SDL_Keycode, int> keyMap = {
        {SDLK_a, 60}, {SDLK_s, 62}, {SDLK_d, 64}, {SDLK_f, 65},
        {SDLK_g, 67}, {SDLK_h, 69}, {SDLK_j, 71}, {SDLK_k, 72}
    };

    SynthEngine engine;

    SDL_AudioSpec ds;
    ds.freq = 44100;
    ds.format = AUDIO_F32SYS;
    ds.channels = 1;
    ds.samples = 1024; 
    ds.callback = SynthEngine::AudioCallback;
    ds.userdata = &engine;

    if (SDL_OpenAudio(&ds, NULL) < 0) return 1;
    SDL_PauseAudio(0);

    bool running = true;
    SDL_Event e;

    std::cout << "--- HARD SYNC SYNTH READY ---" << std::endl;
    std::cout << "A-K: Play Notes | UP/DOWN: Adjust Hard Sync | ESC: Quit" << std::endl;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;

                if (keyMap.count(e.key.keysym.sym)) {
                    int note = keyMap[e.key.keysym.sym];
                    float freq = 440.0f * pow(2.0f, (note - 69) / 12.0f);

                    SDL_LockAudio(); // Protect std::vector from concurrent access
                    Voice* newVoice = new Voice(freq, 1.0f, 44100.0f, note);
                    newVoice->setSlaveFrequency(freq * engine.syncMultiplier);
                    engine.voices.push_back(newVoice);
                    SDL_UnlockAudio();
                }

                if (e.key.keysym.sym == SDLK_UP) {
                    engine.syncMultiplier += 0.2f;
                    std::cout << "Sync Multiplier: " << engine.syncMultiplier << std::endl;
                }
                if (e.key.keysym.sym == SDLK_DOWN) {
                    engine.syncMultiplier = std::max(1.0f, engine.syncMultiplier - 0.2f);
                    std::cout << "Sync Multiplier: " << engine.syncMultiplier << std::endl;
                }
            }

            if (e.type == SDL_KEYUP) {
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

        // --- UI RENDERING (Uses the AudioRingBuffer) ---
        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_RenderClear(renderer);
        
        SDL_SetRenderDrawColor(renderer, 0, 255, 204, 255); // Neon Cyan

        // Get the current write pointer from the audio thread
        size_t writePos = scopeBuffer.getWritePointer();

        // We draw 800 samples from the buffer
        for (int x = 0; x < 799; x++) {
            // Read samples relative to the current write position (last 800 samples)
            // This prevents the "jumping" effect of a static buffer
            float s1 = scopeBuffer.read(writePos + x - 800);
            float s2 = scopeBuffer.read(writePos + x + 1 - 800);

            int y1 = 200 - (int)(s1 * 150);
            int y2 = 200 - (int)(s2 * 150);
            SDL_RenderDrawLine(renderer, x, y1, x + 1, y2);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Target ~60 FPS
    }

    SDL_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}