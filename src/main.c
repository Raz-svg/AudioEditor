#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.c" 

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048

#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()

Color softYellow = (Color){255, 255, 153, 100};  

void draw_waveform(const char *filepath, int num_samples) {
    int centerY = SCREEN_HEIGHT / 2;
    int scale = 100;

    DrawLine(0, centerY, SCREEN_WIDTH, centerY, BLACK);    

    read_wav_header(filepath);
    int16_t *pcm = read_pcm_data(filepath);
    if (!pcm) return;

    for (int i = 0; i < num_samples; i++) {
        int x1 = i * SCREEN_WIDTH / num_samples;
        int y1 = centerY - (pcm[i] / scale);
        int x2 = x1;
        int y2 = centerY;
        DrawLine(x1, y1, x2, y2, softYellow);
    }

    free(pcm);  
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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Waveform");

    SetTargetFPS(60);  

    int filePathCounter = 0;
    char *filePaths[MAX_FILEPATH_RECORDED] = { 0 };
    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
    }

    while (!WindowShouldClose()) {
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();
            for (int i = 0; i < (int)droppedFiles.count; i++) {
                if (filePathCounter < MAX_FILEPATH_RECORDED - 1) {
                    TextCopy(filePaths[filePathCounter], droppedFiles.paths[i]);
                    filePathCounter++;
                }
            }
            UnloadDroppedFiles(droppedFiles);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (filePathCounter == 0) {
            DrawText("Drop your files to this window!", 100, 40, 20, DARKGRAY);
        } else {
            DrawText("Dropped files:", 100, 40, 20, DARKGRAY);
            for (int i = 0; i < filePathCounter; i++) {
                if (i % 2 == 0)
                    DrawRectangle(0, 85 + 40 * i, SCREEN_WIDTH, 40, Fade(LIGHTGRAY, 0.5f));
                else
                    DrawRectangle(0, 85 + 40 * i, SCREEN_WIDTH, 40, Fade(LIGHTGRAY, 0.3f));

                DrawText(filePaths[i], 10, 95 + 40 * i, 10, DARKGRAY);

                read_wav_header(filePaths[i]);
                int num_samples = header.subchunk2size / sizeof(int16_t);
                draw_waveform(filePaths[i], num_samples);
            }

            DrawText("Drop new files...", 100, 110 + 40 * filePathCounter, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow(); 
    free_list();  

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        RL_FREE(filePaths[i]);
    }

    return 0;
}
