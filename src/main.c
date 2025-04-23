#include "util.c" 

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048

#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()

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
        ClearBackground(DARKGRAY);

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
