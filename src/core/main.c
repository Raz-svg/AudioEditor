#include <SDL2/SDL.h>
#include "../audio/util.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void draw_waveform(SDL_Renderer *renderer, int16_t *pcm, int num_samples) {
    int centerY = SCREEN_HEIGHT / 2;
    int scale = 100; // Scaling factor for better visibility

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 1; i < num_samples - 1; i++) {
        int x1 = (i - 1) * SCREEN_WIDTH / num_samples;
        int y1 = centerY - (pcm[i - 1] / scale);
        int x2 = i * SCREEN_WIDTH / num_samples;
        int y2 = centerY - (pcm[i] / scale);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

int main() {
    const char *filename1 = "/home/raz/Desktop/sound_composer/assets/sample1.wav";
    const char *filename2 = "/home/raz/Desktop/sound_composer/assets/sample2.wav";

    read_wav_header(filename1);
    int16_t *pcm1 = read_pcm_data(filename1);
    read_wav_header(filename2);
    int16_t *pcm2 = read_pcm_data(filename2);

    insert(pcm1);
    insert(pcm2);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Initialization Failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Waveform Visualization",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw waveform for the first file
        if (pcm1) draw_waveform(renderer, pcm1, header.subchunk2size / sizeof(int16_t));

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free_list();
    return 0;
}
