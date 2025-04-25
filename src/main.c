#include "util.c" 

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048



int main() {

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Waveform");


    Screen currentScreen = Load;
    Image logo = LoadImage("Group 1 (1).png");
    Texture2D texture1 = LoadTextureFromImage(logo); 

    Image drop =LoadImage("Group 1 (2).png");
    Texture2D texture2 = LoadTextureFromImage(drop);
 
    Vector2 buttonCenter = { (SCREEN_WIDTH - texture1.width) / 2 + texture1.width / 2, (SCREEN_HEIGHT - texture1.height) / 6 + texture1.height / 2 };
    Rectangle buttonBounds = { buttonCenter.x, buttonCenter.y, (float)texture1.width, (float)texture1.height };
    Rectangle dropBounds = { 0, 0, (float)texture1.width, (float)texture1.height };

    float buttonRadius = texture1.width+20 ;

    bool isActiverec = false;

    bool isActivedrop = false;

    UnloadImage(logo); 
    UnloadImage(drop);


    int filePathCounter = 0;
    char *filePaths[MAX_FILEPATH_RECORDED] = { 0 };
    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
    }

    while (!WindowShouldClose()) {
        butterworth_low_filter("/home/raz/Desktop/sound_composer/assests/sample1.wav", "low.wav");
        
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
                 Vector2 mousePos = GetMousePosition();

                 if (CheckCollisionPointRec(mousePos, buttonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                 {
                   isActiverec = !isActiverec;// for toggle
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
                   isActivedrop = !isActivedrop;// for toggle
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
                 DrawText("Real Screen", 10, 10, 20, WHITE);
             
                break;
            case Real:
                    float pulse = 1.0f + 0.1f * sin(GetTime() * 6.0f);
                    if(isActiverec){
                        DrawCircleV(buttonCenter, buttonRadius * pulse, (Color){255, 0, 0, 80*pulse});
                        DrawTexture(texture1,(SCREEN_WIDTH - texture1.width) / 2, (SCREEN_HEIGHT - texture1.height) / 6,WHITE);
                    }
                    else{
                        DrawTexture(texture1,(SCREEN_WIDTH - texture1.width) / 2, (SCREEN_HEIGHT - texture1.height) / 6,WHITE);
                    }
                   
          
                break;
            case Static:
            
                 if (filePathCounter == 0) {
                     const char *message = "Drop your files to this window!";
                     int fontSize = 40;
                     int textWidth = MeasureText(message, fontSize);
                
                    DrawText(message, (SCREEN_WIDTH - textWidth)/2, SCREEN_HEIGHT/2, fontSize, WHITE);
                
                 } 
                 else {

                    if(isActivedrop)
                    {
                        bool activefile=false;
                        Rectangle file = { 0, 0, 350, SCREEN_HEIGHT };
                        DrawRectangleRec(file, Fade(DARKGRAY, 0.5f));
                        DrawText("Dropped files:", 10, 40, 20, WHITE);
                    
                        for (int i = 0; i < filePathCounter; i++) 
                        {
                            Rectangle fileEntry = {0, 85 + 40 * i, 350, 40};
                    
                            
                            if (i % 2 == 0)
                                DrawRectangleRec(fileEntry, Fade(LIGHTGRAY, 0.5f));
                            else
                                DrawRectangleRec(fileEntry, Fade(LIGHTGRAY, 0.3f));
                    
                            DrawText(filePaths[i], 10, 95 + 40 * i, 10, DARKGRAY);
                    
                            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), fileEntry)) {
                                activefile = true;
                                
                            }
                            if(activefile){
                                read_wav_header(filePaths[i]);
                    
                                int num_samples = header.subchunk2size / sizeof(int16_t);
                                draw_waveform(filePaths[i], num_samples);
                            }
                            else{
                                const char *message1 = "Drop your files to this window!";
                                int fontSize1 = 30;
                                int textWidth1 = MeasureText(message1, fontSize1);
                           
                               DrawText(message1, (SCREEN_WIDTH - textWidth1)/2, SCREEN_HEIGHT/2, fontSize1, WHITE);
                            }
                        }
                    }
                    else
                    {
                        DrawTexture(texture2, 0, 0, WHITE);
                        const char *message2 = "Drop your files to this window!";
                        int fontSize2 = 40;
                        int textWidth2 = MeasureText(message2, fontSize2);
                   
                        DrawText(message2, (SCREEN_WIDTH - textWidth2)/2, SCREEN_HEIGHT/2, fontSize2, WHITE);
                    }
                // DrawText("Drop new files...", 10, 110 + 40 * filePathCounter, 20, WHITE);
    
                  break;
        }
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
