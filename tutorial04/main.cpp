#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "Oscillator.h"

// Shared buffer for the oscilloscope visualization
std::vector<float> visualBuffer(2048, 0.0f);

class SynthEngine {
public:
    // Polymorphic pointer to the base class
    Oscillator* osc;
    bool active;

    SynthEngine() : active(false) {
        // Initialize with a default waveform (Square)
        osc = new SquareOsc(440.0, 0.2, 44100.0);
    }

    ~SynthEngine() {
        delete osc;
    }

    // The interface to the sound hardware
    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            float sample = 0.0f;

            // SAFETY CHECK: Only access if key is pressed AND oscillator pointer is valid
            // This prevents Segmentation Faults during waveform swaps
            if (engine->active && engine->osc != nullptr) {
                sample = engine->osc->getNextSample();
            }

            buffer[i] = sample;

            // Fill the visualization buffer for the oscilloscope
            if (i < (int)visualBuffer.size()) {
                visualBuffer[i] = sample;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    // Initialize Audio and Video (Video is required for keyboard focus)
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a window for focus and the visualizer
    SDL_Window* window = SDL_CreateWindow("C++ Synth Visualizer", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_SHOWN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SynthEngine engine;

    SDL_AudioSpec ds;
    ds.freq = 44100;
    ds.format = AUDIO_F32SYS; // 32-bit float audio format
    ds.channels = 1;
    ds.samples = 2048;
    ds.callback = SynthEngine::AudioCallback;
    ds.userdata = &engine;

    if (SDL_OpenAudio(&ds, NULL) < 0) {
        std::cerr << "Audio device error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_PauseAudio(0); // Start the audio thread

    std::cout << "Keys 1-4: Switch Waveform | SPACE: Play Note | ESC: Exit" << std::endl;

    bool running = true;
    SDL_Event e;
    
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_KEYDOWN) {
                Oscillator* nextOsc = nullptr;
                double freq = 440.0;
                double amp = 0.2;
                double sr = 44100.0;

                // THREAD-SAFE WAVEFORM SWAPPING
                // We create the new object first, then swap the pointer
                switch (e.key.keysym.sym) {
                    case SDLK_1: nextOsc = new SquareOsc(freq, amp, sr); break;
                    case SDLK_2: nextOsc = new SawOsc(freq, amp, sr); break;
                    case SDLK_3: nextOsc = new SineOsc(freq, amp, sr); break;
                    case SDLK_4: nextOsc = new TriangleOsc(freq, amp, sr); break;
                    case SDLK_ESCAPE: running = false; break;
                }

                if (nextOsc != nullptr) {
                    Oscillator* oldOsc = engine.osc;
                    
                    // Critical section: Nullify the pointer momentarily so the 
                    // audio callback plays silence instead of crashing
                    engine.osc = nullptr; 
                    delete oldOsc;        // Free memory of the previous oscillator
                    engine.osc = nextOsc; // Assign the new polymorphic object
                }
            }
        }

        // Poll keyboard state for the Spacebar
        const Uint8* state = SDL_GetKeyboardState(NULL);
        engine.active = state[SDL_SCANCODE_SPACE];

        // --- GRAPHICS: OSCILLOSCOPE RENDERING ---
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255); // Dark background
        SDL_RenderClear(renderer);

        // Draw Zero-Line (Grey)
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
        SDL_RenderDrawLine(renderer, 0, 200, 800, 200);

        // Draw Waveform (Green)
        SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255);
        for (int x = 0; x < 799; x++) {
            // Scale sample (-1.0 to 1.0) to pixels (centered at 200)
            int y1 = 200 - (int)(visualBuffer[x] * 150);
            int y2 = 200 - (int)(visualBuffer[x+1] * 150);
            SDL_RenderDrawLine(renderer, x, y1, x + 1, y2);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Sync to ~60 FPS
    }

    // Cleanup resources
    SDL_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}