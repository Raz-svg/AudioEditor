#include <SDL2/SDL.h>
#include <stdint.h>
#include "../audio/util.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 800

void draw_waveform(SDL_Renderer *renderer, int16_t *pcm, int num_samples) {
    int centerY = SCREEN_HEIGHT / 2;
    int scale = 100; // Scaling factor for better visibility

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer,0,centerY,SCREEN_WIDTH,centerY);
    for (int i = 0; i < num_samples; i++) {
        int x1 = i * SCREEN_WIDTH / num_samples;
        int y1 = centerY - (pcm[i] / scale);
        int x2 = x1;
        int y2 = centerY;
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2); 
        SDL_RenderDrawPoint(renderer, x1, y1);

    }

}

int factor = 2;
void zoom(int *num_samples) {
    *num_samples = (*num_samples) / factor;
}

int main() {
    const char *filename1 = "../assests/sample1.wav";
    const char *filename2 = "../assests/sample2.wav";

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

    SDL_Window *window = SDL_CreateWindow("Waveform Visualization", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;
    int samples = header.subchunk2size / sizeof(int16_t);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;
            
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                zoom(&samples);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        if (pcm1) draw_waveform(renderer, pcm1, samples);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free_list();
    return 0;
}

