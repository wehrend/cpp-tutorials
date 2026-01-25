#include <SDL2/SDL.h>
#include <iostream>
#include "Oscillator.h"

class SynthEngine {
public:
    Oscillator osc;
    bool active;

    SynthEngine() : osc(440.0, 0.2, 44100.0), active(false) {}

    static void AudioCallback(void* userdata, Uint8* stream, int len) {
        SynthEngine* engine = static_cast<SynthEngine*>(userdata);
        float* buffer = reinterpret_cast<float*>(stream);
        int length = len / sizeof(float);

        for (int i = 0; i < length; i++) {
            // If active is true, the sample is calculated, otherwise 0.0f (silence)
            // multiply with 0.05f for reasonable quiet sound
            buffer[i] = 0.05f * ( engine->active ? engine->osc.getNextSample() : 0.0f);
        }
    }
};

int main(int argc, char* argv[]) {
    // Initialize audio AND video (for the window/focus)
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a simple window so that we can get the keyboard focus
    SDL_Window* window = SDL_CreateWindow(
        "Synth Focus window", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        300, 200, 
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SynthEngine engine;

    SDL_AudioSpec ds;
    ds.freq = 44100;
    ds.format = AUDIO_F32SYS;
    ds.channels = 1;
    ds.samples = 2048;
    ds.callback = SynthEngine::AudioCallback;
    ds.userdata = &engine;

    if (SDL_OpenAudio(&ds, NULL) < 0) {
        std::cerr << "Audio Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_PauseAudio(0); // start Audio-Thread

    std::cout << "--- SYNTH ready ---" << std::endl;
    std::cout << "1. Click on Window 'Synth focus window'" << std::endl;
    std::cout << "2. Hold space key for sound" << std::endl;
    std::cout << "3. Close window to exit" << std::endl;

    bool running = true;
    SDL_Event e;
    
    while (running) { 
        // Event loop: Important for keeping the window alive
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        // poll keyboard state
        const Uint8* state = SDL_GetKeyboardState(NULL);
        
        // OOP: We set the internal state of our engine from the outside
        if (state[SDL_SCANCODE_SPACE]) {
            if (!engine.active) {
                engine.active = true;
            }
        } else {
            if (engine.active) {
                engine.active = false;
            }
        }

        SDL_Delay(10); // Prevents the CPU load from jumping to 100%
    }

    // clean up
    SDL_CloseAudio();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}