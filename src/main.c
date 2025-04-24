//#include <SDL2/SDL.h>
#include <stdint.h>
#include "util.c"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

/*void draw_waveform(SDL_Renderer *renderer, int16_t *pcm, int num_samples) {
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
}*/
/*void reverse(const char *filename1)
{
    FILE *input = fopen(filename1, "rb");
    if(input == NULL)
    {
        printf("Error opening file: %s\n", filename1);
        return;
    }
    FILE *output = fopen(filename1, "wb");
    if(output == NULL)
    {
        printf("Error opening file: %s\n", filename1);
        return;
    }
    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    while(file_size > sizeof(header))
    {
        int16_t numchannels;
        fseek(input,-8,SEEK_CUR);
        fread(&numchannels, 4, 1, input);
        fwrite(&numchannels, 4, 1, output);
        file_size=file_size- 4;
    }
    fclose(input);
    fclose(output);

}*/
void reverse(const char* input_filename, const char* output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (input == NULL) {
        printf("Error opening input file: %s\n", input_filename);
        return;
    }

    FILE *output = fopen(output_filename, "wb");
    if (output == NULL) {
        printf("Error opening output file: %s\n", output_filename);
        fclose(input);
        return;
    }

    // Copy the header (first 44 bytes) from input to output
    uint8_t header[44];
    fread(header, sizeof(uint8_t), 44, input);
    fwrite(header, sizeof(uint8_t), 44, output);

    // Determine the size of the PCM data
    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    long pcm_data_size = file_size - 44; // Exclude the header
    fseek(input, 44, SEEK_SET); // Move to the start of the PCM data

    // Reverse the PCM data
    int16_t sample;
    long num_samples = pcm_data_size / sizeof(int16_t);
    for (long i = num_samples - 1; i >= 0; i--) {
        fseek(input, 44 + i * sizeof(int16_t), SEEK_SET);
        fread(&sample, sizeof(int16_t), 1, input);
        fwrite(&sample, sizeof(int16_t), 1, output);
    }

    fclose(input);
    fclose(output);
}


int main() {
    const char *filename1 = "../assests/sample1.wav";
    const char *filename2 = "../assests/sample2.wav";

    read_wav_header(filename1);
    int16_t *pcm1 = read_pcm_data(filename1);
    read_wav_header(filename2);
    int16_t *pcm2 = read_pcm_data(filename2);

    //insert(pcm1);
    //insert(pcm2);

    /*if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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
    */
   reverse(filename1,filename2);
   return 0;
}

