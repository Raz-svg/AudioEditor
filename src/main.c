#include "util.c" 

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048



int main() {

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Waveform");

    Screen currentScreen = Load;
    Image logo = LoadImage("Group2.png");
    Texture2D texture1 = LoadTextureFromImage(logo); 
    UnloadImage(logo); 

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
               break;
            case Static:
               
                 if(IsKeyPressed(KEY_ENTER)) {
                     currentScreen = Real;
                 }
                 else if (IsKeyPressed(KEY_SPACE)) {
                     currentScreen = Load;
    
             }
                break;
                default:
                break;
        }
       


        BeginDrawing();
        ClearBackground(DARKGRAY);
       
        switch(currentScreen)
        {
            case Load:
                DrawTexture(texture1,(SCREEN_WIDTH - texture1.width) / 2, (SCREEN_HEIGHT - texture1.height) / 2,WHITE);
             
                break;
            case Real:
                DrawText("Real Screen", 10, 10, 20, WHITE);
                break;
            case Static:
            
                 if (filePathCounter == 0) {
                     const char *message = "Drop your files to this window!";
                     int fontSize = 40;
                     int textWidth = MeasureText(message, fontSize);
                
                    DrawText(message, (SCREEN_WIDTH - textWidth)/2, SCREEN_HEIGHT/2, fontSize, BLACK);
                
                 } 
                 else {
                    DrawText("Dropped files:", 100, 40, 20, BLACK);
                    for (int i = 0; i < filePathCounter; i++) 
                    {
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
    
                  break;
        }

        EndDrawing();
    }
    UnloadTexture(texture1);

    CloseWindow(); 
    free_list();  

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        RL_FREE(filePaths[i]);
    }

    return 0;
}
