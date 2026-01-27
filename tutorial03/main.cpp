#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "Oscillator.h"

// Ein globaler Puffer, damit der Renderer auf die Audiodaten zugreifen kann
// Wir nutzen 400 Samples, passend zur Fensterbreite
std::vector<float> visualBuffer(400, 0.0f);

class SynthEngine {
public:
    Oscillator osc;
    bool active;

    SynthEngine() : osc(440.0, 0.3, 44100.0), active(false) {}

    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            float sample = engine->active ? engine->osc.getNextSample() : 0.0f;
            buffer[i] = sample;

            // Wir füllen den Visualisierungs-Puffer nur mit den ersten 400 Samples
            if (i < visualBuffer.size()) {
                visualBuffer[i] = sample;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

    // Fenster und Renderer erstellen
    SDL_Window* window = SDL_CreateWindow("C++ Synth Visualizer", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300, SDL_WINDOW_SHOWN);
    
    // Der Renderer ist unser "Pinsel" für das Fenster
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
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        engine.active = state[SDL_SCANCODE_SPACE];

        // --- GRAFIK RENDERING ---
        
        // 1. Hintergrund löschen (Schwarz/Dunkelblau)
        SDL_SetRenderDrawColor(renderer, 10, 10, 25, 255);
        SDL_RenderClear(renderer);

        // 2. Nulllinie zeichnen (Grau)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderDrawLine(renderer, 0, 150, 400, 150);

        // 3. Wellenform zeichnen (Neongrün)
        SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
        for (int x = 0; x < (int)visualBuffer.size() - 1; x++) {
            // Wir skalieren den Sample-Wert (-1.0 bis 1.0) auf die Fensterhöhe
            // 150 ist die Mitte des Fensters, 100 ist die Amplitude
            int y1 = 150 - (int)(visualBuffer[x] * 100);
            int y2 = 150 - (int)(visualBuffer[x+1] * 100);
            SDL_RenderDrawLine(renderer, x, y1, x + 1, y2);
        }

        // 4. Alles auf den Bildschirm bringen
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Entspricht etwa 60 FPS
    }

    SDL_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}