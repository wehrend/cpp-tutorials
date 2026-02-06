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

// Globaler Buffer für die Oszilloskop-Anzeige
std::vector<float> visualBuffer(2048, 0.0f);

class SynthEngine {
public:
    std::vector<Voice*> voices;
    LowPassFilter filter;
    SquareOsc lfo;
    
    float masterVolume = 0.2f;
    float syncMultiplier = 2.5f; // Wie viel höher der Slave schwingt

    SynthEngine() : lfo(0.5f, 1.0f, 44100.0f) {
        filter.setCutoff(0.8f);
    }

    ~SynthEngine() {
        for (auto v : voices) delete v;
    }

    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            float mixedSample = 0.0f;

            // Optional: LFO moduliert hier die Filter-Cutoff oder Sync-Frequenz
            // float modulation = engine->lfo.getNextSample();

            for (auto it = engine->voices.begin(); it != engine->voices.end(); ) {
                Voice* v = *it;
                
                // Wir aktualisieren die Slave-Frequenz basierend auf dem globalen Multiplikator
                // masterOsc->frequency * syncMultiplier
                // (Normalerweise greift man hier sauberer über eine Methode zu)
                // v->setSlaveFrequency(v->masterOsc->getFrequency() * engine->syncMultiplier);

                mixedSample += v->getNextSample();

                if (v->env.getState() == OFF) {
                    delete v;
                    it = engine->voices.erase(it);
                } else {
                    ++it;
                }
            }

            // Globaler Filter & Master Volume
            float filtered = engine->filter.process(mixedSample);
            buffer[i] = filtered * engine->masterVolume;

            if (i < (int)visualBuffer.size()) visualBuffer[i] = buffer[i];
        }
    }
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("C++ Hard Sync Synth", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // QWERTY/QWERTZ Mapping
    std::map<SDL_Keycode, int> keyMap = {
        {SDLK_a, 60}, {SDLK_s, 62}, {SDLK_d, 64}, {SDLK_f, 65},
        {SDLK_g, 67}, {SDLK_h, 69}, {SDLK_j, 71}, {SDLK_k, 72}
    };

    SynthEngine engine;

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

    std::cout << "--- HARD SYNC SYNTH READY ---" << std::endl;
    std::cout << "A-K: Noten spielen" << std::endl;
    std::cout << "UP/DOWN: Sync-Frequenz (Slave) anpassen" << std::endl;
    std::cout << "ESC: Beenden" << std::endl;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;

                if (keyMap.count(e.key.keysym.sym)) {
                    int note = keyMap[e.key.keysym.sym];
                    float freq = 440.0f * pow(2.0f, (note - 69) / 12.0f);

                    SDL_LockAudio();
                    Voice* newVoice = new Voice(freq, 1.0f, 44100.0f, note);
                    // Initialen Sync-Faktor setzen
                    newVoice->setSlaveFrequency(freq * engine.syncMultiplier);
                    engine.voices.push_back(newVoice);
                    SDL_UnlockAudio();
                }

                // Sync-Frequenz steuern
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

        // Alle aktiven Stimmen auf den neuen Sync-Multiplikator aktualisieren
        SDL_LockAudio();
        for (auto v : engine.voices) {
            // Wir wissen, dass masterOsc die Grundfrequenz hat
            // Für diesen einfachen Code nehmen wir an, der Master ist SawOsc
            // v->setSlaveFrequency(v->masterFreq * engine.syncMultiplier);
        }
        SDL_UnlockAudio();

        // Zeichnen
        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_RenderClear(renderer);
        
        SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255); // Orange für "heißen" Sync-Sound
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