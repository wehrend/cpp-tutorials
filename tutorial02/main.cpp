#ifdef _WIN32
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif
#include <cmath>
#include <iostream>
#include <atomic>

static constexpr double SAMPLE_RATE = 44100.0;
static constexpr double FREQUENCY   = 440.0;
static constexpr double TWO_PI      = 6.28318530717958647692;

struct AudioState {
    std::atomic<bool> playing{false}; // set by main thread, read by audio thread
    double phase = 0.0;               // only touched by audio thread
};

void audioCallback(void* userdata, Uint8* stream, int len)
{
    auto* state = static_cast<AudioState*>(userdata);

    float* buffer = reinterpret_cast<float*>(stream);
    int samples = len / sizeof(float);

    // If not playing, output silence
    if (!state->playing.load(std::memory_order_relaxed)) {
        for (int i = 0; i < samples; ++i) buffer[i] = 0.0f;
        return;
    }

    const double phaseInc = TWO_PI * FREQUENCY / SAMPLE_RATE;

    for (int i = 0; i < samples; ++i) {
        buffer[i] = 0.05f * static_cast<float>(std::sin(state->phase));
        state->phase += phaseInc;

        if (state->phase >= TWO_PI)
            state->phase -= TWO_PI;
    }
}

int main(int /*argc*/, char** /*argv*/)
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // Small window just to receive keyboard events
    SDL_Window* win = SDL_CreateWindow(
        "Hold SPACE to play",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        400, 120,
        SDL_WINDOW_SHOWN
    );
    if (!win) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    AudioState state;

    SDL_AudioSpec want{};
    want.freq     = static_cast<int>(SAMPLE_RATE);
    want.format   = AUDIO_F32SYS;
    want.channels = 1;          // mono
    want.samples  = 512;
    want.callback = audioCallback;
    want.userdata = &state;

    SDL_AudioSpec have{};
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (!dev) {
        std::cerr << "Audio open failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_PauseAudioDevice(dev, 0); // start audio thread/callback

    std::cout << "Hold SPACE to play 440 Hz. Press ESC or close window to quit.\n";

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN && !e.key.repeat) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (e.key.keysym.sym == SDLK_SPACE)  state.playing.store(true, std::memory_order_relaxed);
            } else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_SPACE)  state.playing.store(false, std::memory_order_relaxed);
            }
        }

        SDL_Delay(1); // tiny sleep to avoid busy-waiting
    }

    SDL_CloseAudioDevice(dev);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
