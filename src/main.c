#include "util.h"     // Include your header first // Include recorder header
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h> // For bool type

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1000

int selectedFileIndex = -1;

int reverse_count = 0;
int lowpass_count = 0;
int merge_count = 0;




int main(int argc, char *argv[]) {

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Waveform");
    InitAudioDevice(); 

    Screen currentScreen = Load;
    Image logo = LoadImage("Group 1 (1).png");
    if (logo.data == NULL) { TraceLog(LOG_WARNING, "Failed to load logo image"); CloseWindow(); return 1; }
    Texture2D texture1 = LoadTextureFromImage(logo);
    UnloadImage(logo);

    Image dockfile=LoadImage("/home/raz/Desktop/sound_composer/src/Group 1 (7).png");
    if (dockfile.data == NULL) { TraceLog(LOG_WARNING, "Failed to load dockfile image"); CloseWindow(); return 1; }
    Texture2D texture3 = LoadTextureFromImage(dockfile);
    UnloadImage(dockfile);

    Image merge=LoadImage("/home/raz/Desktop/sound_composer/src/Group 1 (8).png");
    if(merge.data == NULL) { TraceLog(LOG_WARNING, "Failed to load merge image"); CloseWindow(); return 1; }
    Texture2D texture4 = LoadTextureFromImage(merge);
    UnloadImage(merge);

    Image lowfile=LoadImage("/home/raz/Desktop/sound_composer/src/Group 1 (9).png");
    if(lowfile.data == NULL) { TraceLog(LOG_WARNING, "Failed to load reversefile image"); CloseWindow(); return 1; }
    Texture2D texture5 = LoadTextureFromImage(lowfile);
    UnloadImage(lowfile);

    Image reversefile=LoadImage("/home/raz/Desktop/sound_composer/src/Group 1 (10).png");
    if(reversefile.data == NULL) { TraceLog(LOG_WARNING, "Failed to load reversefile image"); CloseWindow(); return 1; }
    Texture2D texture6 = LoadTextureFromImage(reversefile);
    UnloadImage(reversefile);

    float buttonX = (SCREEN_WIDTH - texture1.width) / 2.0f;
    float buttonY = (SCREEN_HEIGHT - texture1.height) / 6.0f;
    Rectangle buttonBounds = { buttonX, buttonY, (float)texture1.width, (float)texture1.height };
    Vector2 buttonCenter = { buttonX + texture1.width / 2.0f, buttonY + texture1.height / 2.0f };
    float buttonRadius = texture1.width / 2.0f + 90.0f ;

    
    float dock_width = 50.0f;
    float padding = 30.0f;
    float current_y_offset = padding;

    Rectangle dropBounds = { (dock_width - (float)texture3.width) / 2.0f, current_y_offset, (float)texture3.width, (float)texture3.height };
    current_y_offset += (float)texture3.height + padding;

    Rectangle mergeaBounds = { (dock_width - (float)texture4.width) / 2.0f, current_y_offset, (float)texture4.width, (float)texture4.height };
    current_y_offset += (float)texture4.height + padding;

    Rectangle lowfileBounds = { (dock_width - (float)texture5.width) / 2.0f, current_y_offset, (float)texture5.width, (float)texture5.height };
    current_y_offset += (float)texture5.height + padding;

    Rectangle reversefileBounds = { (dock_width - (float)texture6.width) / 2.0f, current_y_offset, (float)texture6.width, (float)texture6.height };


    bool isActiverec = false;
    bool isActivedrop = false;


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
          
             if (isActivedrop) selectedFileIndex = -1;
        }

       
        switch(currentScreen){
            case Load:
                if(IsKeyPressed(KEY_ENTER)) {
                    currentScreen = Real;
                }
                else if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = Static;
                    selectedFileIndex = -1; 
                }
                break;

            case Real:
                 if(IsKeyPressed(KEY_ENTER)) {
                     currentScreen = Static;
                     selectedFileIndex = -1; 
                 }
                 else if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = Load;
                 }

               
                 Vector2 mousePos = GetMousePosition();
                 if (CheckCollisionPointRec(mousePos, buttonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                 {
                   isActiverec = !isActiverec;

                    if (isActiverec) {
                          StartAudioRecording();
                          TraceLog(LOG_INFO, "Started recording...");
                     } else {

                         StopAudioRecording();
                         TraceLog(LOG_INFO, "Stopped recording.");
                          // TODO: Process the recorded audio (load from file or memory)
                        }
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
                 
                   if (!isActivedrop) selectedFileIndex = -1;
                 }

                 if (CheckCollisionPointRec(mousePos2, mergeaBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                 {
                   if (filePathCounter > 0) {
                       TraceLog(LOG_INFO, "Merge icon clicked. Merging all dropped files.");

                       
                       free_list();

                      
                       for (int i = 0; i < filePathCounter; i++) {
                           const char* currentFileToMerge = filePaths[i];
                           read_wav_header(currentFileToMerge); // Read header before reading PCM
                           int16_t* pcm_data = read_pcm_data(currentFileToMerge);

                           if (pcm_data) {
                               insert(pcm_data);
                               TraceLog(LOG_INFO, "Added %s to merge list.", GetFileName(currentFileToMerge));
                           } else {
                               TraceLog(LOG_WARNING, "Failed to read PCM data for merging: %s", GetFileName(currentFileToMerge));
                           }
                       }

                       // Write the merged data to a new file with counter
                       char mergedOutputFile[MAX_FILEPATH_SIZE];
                       snprintf(mergedOutputFile, MAX_FILEPATH_SIZE, "merged_audio_%d.wav", merge_count);
                       write_pcm_data((char*)mergedOutputFile); // Cast to char* to match function signature
                       TraceLog(LOG_INFO, "Merged files into: %s", mergedOutputFile);

                       // Increment merge counter
                       merge_count++;

                       // Clear the merge list after writing
                       free_list();

                       // TODO: You might want to add the merged output file to your displayed file list
                       // or handle the result as needed.

                   } else {
                       TraceLog(LOG_WARNING, "No files dropped to merge.");
                   }
                 }

                
                 if (CheckCollisionPointRec(mousePos2, lowfileBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                 {
                   if (selectedFileIndex != -1) {
                       const char* inputFile = filePaths[selectedFileIndex];
                       char outputFile[MAX_FILEPATH_SIZE];
                       // Include counter in output filename
                       snprintf(outputFile, MAX_FILEPATH_SIZE, "%s_lowpass_%d.wav", GetFileNameWithoutExt(inputFile), lowpass_count);
                       // Using butterworth_low_filter as an example
                       butterworth_low_filter(inputFile, outputFile);
                       TraceLog(LOG_INFO, "Applied low-pass filter to: %s -> %s", inputFile, outputFile);

                       // Increment lowpass counter
                       lowpass_count++;

                       // TODO: Add the output file to the displayed file list if desired
                   } else {
                       TraceLog(LOG_WARNING, "No file selected to apply low-pass filter.");
                   }
                 }

                
                 if (CheckCollisionPointRec(mousePos2, reversefileBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                 {
                   if (selectedFileIndex != -1) {
                       const char* inputFile = filePaths[selectedFileIndex];
                       char outputFile[MAX_FILEPATH_SIZE];
                       // Include counter in output filename
                       snprintf(outputFile, MAX_FILEPATH_SIZE, "%s_reversed_%d.wav", GetFileNameWithoutExt(inputFile), reverse_count);
                       reverse(inputFile, outputFile);
                       TraceLog(LOG_INFO, "Reversed file: %s -> %s", inputFile, outputFile);

                    
                       reverse_count++;

                      
                   } else {
                       TraceLog(LOG_WARNING, "No file selected to reverse.");
                   }
                 }

              
                if (isActivedrop) {
                    Vector2 mousePosFiles = GetMousePosition();
                    for (int i = 0; i < filePathCounter; i++)
                    {
                        Rectangle fileEntry = {50, 85.0f + 40.0f * i, 350.0f, 40.0f};
                        if (CheckCollisionPointRec(mousePosFiles, fileEntry) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            selectedFileIndex = i;
                        }
                    }
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
            {
                // Load screen drawing
                const char* loaded = "Load Screen - Press [Enter] for Real, [Space] for Static";
                int fontSize0 = 20;
                int textWidth0 = MeasureText(loaded, fontSize0);
                DrawText(loaded, (SCREEN_WIDTH - textWidth0) / 2, SCREEN_HEIGHT / 2, fontSize0, WHITE);
            } break;

            case Real:
            {
               
                if(isActiverec){
                    float pulse = 1.0f + 0.1f * sinf(GetTime() * 6.0f);
                    DrawCircleV(buttonCenter, buttonRadius * pulse, Fade((Color){255, 0, 0, 255}, 0.3f * pulse));
                    DrawTexture(texture1, (int)buttonX, (int)buttonY, WHITE);
                }
                else{
                    DrawTexture(texture1, (int)buttonX, (int)buttonY, WHITE);
                }
            } break;

            case Static:
            {
            
                Color softgray = (Color){30, 34, 42, 100};
                Rectangle dock = { 0, 0, 50, (float)SCREEN_HEIGHT };
                DrawRectangleRec(dock, Fade(softgray, 0.5f));

              
                 float dock_width = 50.0f;
                 float padding = 30.0f;
                 float current_y_offset = padding;

                 DrawTexture(texture3, (int)((dock_width - (float)texture3.width) / 2.0f), (int)current_y_offset, WHITE);
                 current_y_offset += (float)texture3.height + padding;

                 DrawTexture(texture4, (int)((dock_width - (float)texture4.width) / 2.0f), (int)current_y_offset, WHITE);
                 current_y_offset += (float)texture4.height + padding;

                 DrawTexture(texture5, (int)((dock_width - (float)texture5.width) / 2.0f), (int)current_y_offset, WHITE);
                 current_y_offset += (float)texture5.height + padding;

                 DrawTexture(texture6, (int)((dock_width - (float)texture6.width) / 2.0f), (int)current_y_offset, WHITE);


           
                 if (isActivedrop)
                 {
                     Rectangle filePanel = { 50, 0, 350, (float)SCREEN_HEIGHT };
                     DrawRectangleRec(filePanel, softgray);
                     DrawText("Dropped files:", 60, 40, 20, DARKGRAY);

                     for (int i = 0; i < filePathCounter; i++)
                     {
                         Rectangle fileEntry = {50, 85.0f + 40.0f * i, 350.0f, 40.0f};

                         
                         Vector2 mousePosFiles = GetMousePosition();
                         if (CheckCollisionPointRec(mousePosFiles, fileEntry)) {
                             DrawRectangleRec(fileEntry, Fade(YELLOW, 0.3f));
                         }
                         else if (selectedFileIndex == i) {
                             DrawRectangleRec(fileEntry, Fade(BLUE, 0.3f));
                         }
                         else if (i % 2 == 0) {
                             DrawRectangleRec(fileEntry, Fade(DARKGRAY, 0.3f));
                         } else {
                             DrawRectangleRec(fileEntry, Fade(DARKGRAY, 0.1f));
                         }

                         DrawText(GetFileName(filePaths[i]), 60, (int)(fileEntry.y + 10), 20, WHITE);
                     }

                     // Draw waveform for selected file
                     if (selectedFileIndex != -1) {
                         const char* currentSelectedFilePath = filePaths[selectedFileIndex];

                       
                         read_wav_header(currentSelectedFilePath); // Ensure header is read for the selected file
                         if (header.subchunk2size > 0 && header.bitspersample == 16) {
                            int num_samples = header.subchunk2size / (header.bitspersample / 8);
                            draw_waveform(currentSelectedFilePath, num_samples); // draw_waveform reads PCM data internally
                         } else {
                             DrawText("Cannot display waveform for this file", 410, SCREEN_HEIGHT / 2, 20, RED);
                         }
                     } else {
                         const char *message1 = "Select a file";
                         int fontSize1 = 30;
                         int textWidth1 = MeasureText(message1, fontSize1);
                         DrawText(message1, 410 + (SCREEN_WIDTH - 410 - textWidth1)/2, SCREEN_HEIGHT/2 - fontSize1/2, fontSize1, GRAY);
                     }

                 }
                 else if(filePathCounter > 0 && !isActivedrop)
                 {
                     
                      const char *message2 = "Drop more files or click icon";
                      int fontSize2 = 30;
                      int textWidth2 = MeasureText(message2, fontSize2);
                      DrawText(message2, (SCREEN_WIDTH - textWidth2)/2, SCREEN_HEIGHT/2 - fontSize2/2, fontSize2, GRAY);

                    
                      selectedFileIndex = -1;
                 }

              
                 if (filePathCounter == 0 && !isActivedrop) {
                      const char *message = "Drop your files to this window!";
                      int fontSize = 30;
                      int textWidth = MeasureText(message, fontSize);
                      DrawText(message, (SCREEN_WIDTH - textWidth)/2, SCREEN_HEIGHT/2 - fontSize/2, fontSize, GRAY);
                 }


            } break; 

            default: break; 
        } 
        EndDrawing(); 

    } 

    // --- Cleanup ---
    UnloadTexture(texture1);
    UnloadTexture(texture3);
    UnloadTexture(texture4);
    UnloadTexture(texture5);
    UnloadTexture(texture6);

    // Free allocated memory for file paths
    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        RL_FREE(filePaths[i]);
    }

    CloseAudioDevice(); // Close audio device
    CloseWindow(); // Close Raylib window

    return 0; // Exit successfully
}