#include "raylib.h"
#include <stdint.h>
#include "util.c"

#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()
Color softYellow = (Color){255, 255, 153, 100};  // 100 alpha for transparency


void draw_waveform(int16_t *pcm, int num_samples) {
    int centerY = SCREEN_HEIGHT / 2;
    int scale = 100;

   
    DrawLine( 0, centerY, 800,centerY,BLACK);    
    for (int i = 0; i < num_samples; i++) {
        int x1 = i * SCREEN_WIDTH / num_samples;
        int y1 = centerY - (pcm[i] / scale);
        int x2 = x1;
        int y2 = centerY;
        DrawLine( x1,y1, x2, y2,softYellow);

    }

}

int factor = 2;
void zoom(int *num_samples) {
    *num_samples = (*num_samples) / factor;
}

void reverse(const char *input_file, const char *output_file) {
    
    read_wav_header(input_file);

   
    int16_t *data = read_pcm_data(input_file);
    if (!data) return;

    int num_samples = header.subchunk2size / sizeof(int16_t);

    
    for (int i = 0; i < num_samples / 2; i++) {
        int16_t temp = data[i];
        data[i] = data[num_samples - i - 1];
        data[num_samples - i - 1] = temp;
    }

    
    FILE *out = fopen(output_file, "wb");
    if (!out) {
        printf("Error opening output file: %s\n", output_file);
        free(data);
        return;
    }

    fwrite(&header, sizeof(WAVHeader), 1, out);

    fwrite(data, sizeof(int16_t), num_samples, out);

    fclose(out);
    free(data);
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

    write_pcm_data();
    reverse(filename1, "reversed_output.wav");


    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_VSYNC_HINT);



    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Waveform");
    SetTargetFPS(60);

    int num_samples = header.subchunk2size / sizeof(int16_t);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GRAY);

        draw_waveform(pcm1, num_samples);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            zoom(&num_samples);
        }

        EndDrawing();
    }

    CloseWindow(); 
    free_list();
    return 0;
}

