#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "util.c"

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048


int main() {

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Waveform");

    Screen currentScreen = Load;
    Image logo = LoadImage("Group 1 (1).png");
    if (logo.data == NULL) { TraceLog(LOG_WARNING, "Failed to load logo image"); return 1; }
    Texture2D texture1 = LoadTextureFromImage(logo);

    Image drop =LoadImage("Group 1 (2).png");
    if (drop.data == NULL) { TraceLog(LOG_WARNING, "Failed to load drop image"); }
    Texture2D texture2 = LoadTextureFromImage(drop);

    Image dockfile=LoadImage("/home/raz/Desktop/sound_composer/src/Group 1 (7).png");
    if (dockfile.data == NULL) { TraceLog(LOG_WARNING, "Failed to load dockfile image"); return 1; }
    Texture2D texture3 = LoadTextureFromImage(dockfile);

    float buttonX = (SCREEN_WIDTH - texture1.width) / 2.0f;
    float buttonY = (SCREEN_HEIGHT - texture1.height) / 6.0f;
    Rectangle buttonBounds = { buttonX, buttonY, (float)texture1.width, (float)texture1.height };
    Vector2 buttonCenter = { buttonX + texture1.width / 2.0f, buttonY + texture1.height / 2.0f };
    float buttonRadius = texture1.width / 2.0f + 90.0f ;

    Rectangle dropBounds = { 0, 0, (float)texture3.width, (float)texture3.height };

    bool isActiverec = false;
    bool isActivedrop = false;

    UnloadImage(logo);
    UnloadImage(drop);
    UnloadImage(dockfile);

    int filePathCounter = 0;
    char *filePaths[MAX_FILEPATH_RECORDED] = { 0 };
    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
        if (filePaths[i] == NULL) {
            TraceLog(LOG_ERROR, "Failed to allocate memory for file paths");
            for (int j = 0; j < i; j++) RL_FREE(filePaths[j]);
            CloseWindow();
            return 1;
        }
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();
            for (unsigned int i = 0; i < droppedFiles.count; i++) {
                if (filePathCounter < MAX_FILEPATH_RECORDED) {
                    TextCopy(filePaths[filePathCounter], droppedFiles.paths[i]);
                    filePathCounter++;
                } else {
                    TraceLog(LOG_WARNING, "Maximum number of recorded file paths reached.");
                    break;
                }
            }
            UnloadDroppedFiles(droppedFiles);
        }

        switch(currentScreen){
            case Load:
                if(IsKeyPressed(KEY_ENTER)) {
                    currentScreen = Real;
                }
                else if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = Static;
                }
                break;

            case Real:
                 if(IsKeyPressed(KEY_ENTER)) {
                     currentScreen = Static;
                 }
                 else if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = Load;
                 }

                 Vector2 mousePos = GetMousePosition();
                 if (CheckCollisionPointRec(mousePos, buttonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                 {
                   isActiverec = !isActiverec;
                 }
               break;

            case Static:
                 if(IsKeyPressed(KEY_ENTER)) {
                     currentScreen = Real;
                 }
                 else if (IsKeyPressed(KEY_SPACE)) {
                     currentScreen = Load;
                }
                Vector2 mousePos2 = GetMousePosition();
                 if (CheckCollisionPointRec(mousePos2, dropBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                 {
                   isActivedrop = !isActivedrop;
                 }
                break;

            default:
                break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch(currentScreen)
        {
            case Load:
                 DrawText("Load Screen - Press [Enter] for Real, [Space] for Static", 10, 10, 20, WHITE);
                break;

            case Real:
                 DrawText("Real Screen - Press [Enter] for Static, [Space] for Load", 10, 10, 20, WHITE);
                    if(isActiverec){
                        float pulse = 1.0f + 0.1f * sinf(GetTime() * 6.0f);
                        DrawCircleV(buttonCenter, buttonRadius * pulse, Fade((Color){255, 0, 0, 255}, 0.3f * pulse));
                        DrawTexture(texture1, (int)buttonX, (int)buttonY, WHITE);
                    }
                    else{
                        DrawTexture(texture1, (int)buttonX, (int)buttonY, WHITE);
                    }
                break;

            case Static:
                 Rectangle dock = { 0, 0, 50, (float)SCREEN_HEIGHT };
                 DrawRectangleRec(dock, Fade(softgray, 0.5f));

                 DrawTexture(texture3, (50 - texture3.width) / 2, 10, WHITE);

                 if (filePathCounter == 0 && !isActivedrop) {
                     const char *message = "Drop your files to this window!";
                     int fontSize = 30;
                     int textWidth = MeasureText(message, fontSize);
                     DrawText(message, (SCREEN_WIDTH - textWidth)/2, SCREEN_HEIGHT/2 - fontSize/2, fontSize, GRAY);
                 }
                 else {
                    if(isActivedrop)
                    {
                        Rectangle filePanel = { 50, 0, 350, (float)SCREEN_HEIGHT };
                        DrawRectangleRec(filePanel, softgray);
                        DrawText("Dropped files:", 60, 40, 20, DARKGRAY);

                        bool activefile = false;
                        int activeFileIndex = -1;

                        for (int i = 0; i < filePathCounter; i++)
                        {
                            Rectangle fileEntry = {50, 85.0f + 40.0f * i, 350.0f, 40.0f};

                            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), fileEntry)) {
                                activefile = true;
                                activeFileIndex = i;
                            }

                            if (CheckCollisionPointRec(GetMousePosition(), fileEntry)) {
                                DrawRectangleRec(fileEntry, Fade(YELLOW, 0.3f));
                            } else if (i % 2 == 0) {
                                DrawRectangleRec(fileEntry, Fade(LIGHTGRAY, 0.5f));
                            } else {
                                DrawRectangleRec(fileEntry, Fade(LIGHTGRAY, 0.3f));
                            }

                            DrawText(GetFileName(filePaths[i]), 60, (int)(fileEntry.y + 10), 20, DARKGRAY);

                            if (activefile && activeFileIndex == i) {
                                read_wav_header(filePaths[i]);
                                if (header.subchunk2size > 0 && header.bitspersample == 16) {
                                   int num_samples = header.subchunk2size / (header.bitspersample / 8);
                                   draw_waveform(filePaths[i], num_samples);
                                } else {
                                    DrawText("Cannot display waveform for this file", 410, SCREEN_HEIGHT / 2, 20, RED);
                                }
                                activefile = false;
                            }
                        }
                        if (activeFileIndex == -1) {
                             const char *message1 = "Select a file";
                             int fontSize1 = 30;
                             int textWidth1 = MeasureText(message1, fontSize1);
                             DrawText(message1, 410 + (SCREEN_WIDTH - 410 - textWidth1)/2, SCREEN_HEIGHT/2 - fontSize1/2, fontSize1, GRAY);
                        }
                    }
                    else
                    {
                        const char *message2 = "Drop more files or click icon";
                        int fontSize2 = 30;
                        int textWidth2 = MeasureText(message2, fontSize2);
                        DrawText(message2, (SCREEN_WIDTH - textWidth2)/2, SCREEN_HEIGHT/2 - fontSize2/2, fontSize2, GRAY);
                    }
                 }
                 break;
        }

        EndDrawing();
    }

    UnloadTexture(texture1);
    UnloadTexture(texture2);
    UnloadTexture(texture3);

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        RL_FREE(filePaths[i]);
    }

    CloseWindow();

    return 0;
}