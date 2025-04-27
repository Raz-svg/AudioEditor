#include "util.h"     
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>   
#include <math.h>     

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1000


int selectedFileIndex = -1;
int reverse_count = 0;
int lowpass_count = 0;
int merge_count = 0;
int recording_count = 0;
char recordedFileName[MAX_FILEPATH_SIZE] = {0};


int main() {



    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Waveform");
    InitAudioDevice();
     if (!IsAudioDeviceReady()) {
         TraceLog(LOG_ERROR, "Audio device failed to initialize.");
         CloseWindow();
         return 1;
    }

    
    Screen currentScreen = Load;

    Image logo = LoadImage("../../../assests/Group 1 (1).png");
    if (logo.data == NULL) { TraceLog(LOG_WARNING, "Failed to load logo image"); CloseWindow(); return 1; }
    Texture2D texture1 = LoadTextureFromImage(logo);
    UnloadImage(logo);

    Image dockfile=LoadImage("../../../assests/Group 1 (7).png");
    if (dockfile.data == NULL) { TraceLog(LOG_WARNING, "Failed to load dockfile image"); CloseWindow(); return 1; }
    Texture2D texture3 = LoadTextureFromImage(dockfile);
    UnloadImage(dockfile);

    Image merge=LoadImage("../../../assests/Group 1 (8).png");
    if(merge.data == NULL) { TraceLog(LOG_WARNING, "Failed to load merge image"); CloseWindow(); return 1; }
    Texture2D texture4 = LoadTextureFromImage(merge);
    UnloadImage(merge);

    Image lowfile=LoadImage("../../../assests/Group 1 (9).png");
    if(lowfile.data == NULL) { TraceLog(LOG_WARNING, "Failed to load lowfile image"); CloseWindow(); return 1; }
    Texture2D texture5 = LoadTextureFromImage(lowfile);
    UnloadImage(lowfile);

    Image reversefile=LoadImage("../../../assests/Group 1 (10).png");
    if(reversefile.data == NULL) { TraceLog(LOG_WARNING, "Failed to load reversefile image"); CloseWindow(); return 1; }
    Texture2D texture6 = LoadTextureFromImage(reversefile);
    UnloadImage(reversefile);

    // Filter level images
    Image level1=LoadImage("../../../assests/Group 1 (17).png");
    if(level1.data == NULL) { TraceLog(LOG_WARNING, "Failed to load level1 image"); CloseWindow(); return 1; }
    Texture2D texture7 = LoadTextureFromImage(level1);
    UnloadImage(level1);

    Image level2=LoadImage("../../../assests/Group 3 (2).png");
    if(level2.data == NULL) { TraceLog(LOG_WARNING, "Failed to load level2 image"); CloseWindow(); return 1; }
    Texture2D texture8 = LoadTextureFromImage(level2);
    UnloadImage(level2);

    Image level3=LoadImage("../../../assests/Group 6.png");
    if(level3.data == NULL) { TraceLog(LOG_WARNING, "Failed to load level3 image"); CloseWindow(); return 1; }
    Texture2D texture9 = LoadTextureFromImage(level3);
    UnloadImage(level3);

    Image level4=LoadImage("../../../assests/Group 5.png");
    if(level4.data == NULL) { TraceLog(LOG_WARNING, "Failed to load level4 image"); CloseWindow(); return 1; }
    Texture2D texture10 = LoadTextureFromImage(level4);
    UnloadImage(level4);


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

    Rectangle level1Bounds = { (SCREEN_WIDTH/2.0f) - 300 - texture7.width/2.0f+50, 250, (float)texture7.width, (float)texture7.height };
    Rectangle level2Bounds = { (SCREEN_WIDTH/2.0f) + 300 - texture8.width/2.0f+50, 250, (float)texture8.width, (float)texture8.height };
    Rectangle level3Bounds = { (SCREEN_WIDTH/2.0f) - 300 - texture9.width/2.0f+50, 600, (float)texture9.width, (float)texture9.height };
    Rectangle level4Bounds = { (SCREEN_WIDTH/2.0f) + 300 - texture10.width/2.0f+50, 600, (float)texture10.width, (float)texture10.height };
    

    bool isActiverec = false;
    bool isActivedrop = false;
    bool isfilter = false; 


    int filePathCounter = 0;
    char *filePaths[MAX_FILEPATH_RECORDED] = { 0 };
    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
        if (filePaths[i] == NULL) {
            TraceLog(LOG_ERROR, "Failed to allocate memory for file paths");
            for (int j = 0; j < i; j++) RL_FREE(filePaths[j]);
            CloseAudioDevice(); 
            CloseWindow();
            return 1;
        }
    }


    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();
            
            int capacity = MAX_FILEPATH_RECORDED - filePathCounter;
            int filesToAdd = (droppedFiles.count < capacity) ? droppedFiles.count : capacity;

            for (unsigned int i = 0; i < filesToAdd; i++) {
                TextCopy(filePaths[filePathCounter], droppedFiles.paths[i]);
                TraceLog(LOG_INFO,"Dropped file added: %s", filePaths[filePathCounter]);
                filePathCounter++;
            }
             if (droppedFiles.count > filesToAdd) {
                TraceLog(LOG_WARNING, "Maximum number of file paths reached (%d). %d files were not added.", MAX_FILEPATH_RECORDED, droppedFiles.count - filesToAdd);
            }
            UnloadDroppedFiles(droppedFiles);

            // if (isActivedrop) selectedFileIndex = -1;
        }
        switch(currentScreen){
            case Load:
                if(IsKeyPressed(KEY_ENTER)) {
                    currentScreen = Real;
                }
                else if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = Static;
            
                    isActivedrop = false;
                    isfilter = false;
                    // selectedFileIndex = -1; 
                }
                break; 

            case Real:
                 if(IsKeyPressed(KEY_ENTER)) {
                     currentScreen = Static;
                    
                     isActivedrop = false;
                     isfilter = false;
                     
                     if (isActiverec) {
                         StopAudioRecording();
                         isActiverec = false;
                         TraceLog(LOG_INFO, "Stopped recording due to screen change.");
                    
                     }
                 }
                 else if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = Load;
                    
                     if (isActiverec) {
                         StopAudioRecording();
                         isActiverec = false;
                         TraceLog(LOG_INFO, "Stopped recording due to screen change.");
                     }
                 }

                 
                 Vector2 mousePos = GetMousePosition();
                 if (CheckCollisionPointRec(mousePos, buttonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                 {
                   isActiverec = !isActiverec; 

                    if (isActiverec) {
                          
                          snprintf(recordedFileName, MAX_FILEPATH_SIZE, "real_time_recording_%d.wav", recording_count);
                          StartAudioRecording(); 
                          TraceLog(LOG_INFO, "Started recording to: %s", recordedFileName);
                     } else {
                         
                         StopAudioRecording(); 
                         TraceLog(LOG_INFO, "Stopped recording.");

                        
                         if (filePathCounter < MAX_FILEPATH_RECORDED) {
                            TextCopy(filePaths[filePathCounter], recordedFileName);
                            filePathCounter++;
                            recording_count++; 
                         } else {
                             TraceLog(LOG_WARNING, "Maximum number of recorded file paths reached. Cannot add new recording.");
                         }
                         recordedFileName[0] = '\0';
                        }
                     }
               break; 

            case Static:
                 if(IsKeyPressed(KEY_ENTER)) {
                     currentScreen = Real;
                     
                     isActivedrop = false;
                     isfilter = false;
                 }
                 else if (IsKeyPressed(KEY_SPACE)) {
                     currentScreen = Load;
                     
                     isActivedrop = false;
                     isfilter = false;
                 }

                
                Vector2 mousePos2 = GetMousePosition();

        
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                
                    if (CheckCollisionPointRec(mousePos2, dropBounds)) {
                       isActivedrop = !isActivedrop;
                       if (isActivedrop) isfilter = false; 
                       TraceLog(LOG_INFO, "File list panel toggled: %s", isActivedrop ? "ON" : "OFF");
                       // if (!isActivedrop) selectedFileIndex = -1;
                    }
        
                    else if (CheckCollisionPointRec(mousePos2, mergeaBounds)) {
                       if (filePathCounter > 0) {
                           TraceLog(LOG_INFO, "Merge icon clicked. Merging %d files.", filePathCounter);

                           free_list(); 

                           int filesAddedToMerge = 0;
                           for (int i = 0; i < filePathCounter; i++) {
                                const char* currentFileToMerge = filePaths[i];
                            
                                if (FileExists(currentFileToMerge) && IsFileExtension(currentFileToMerge, ".wav")) {
                                    read_wav_header(currentFileToMerge);
                                    int16_t* pcm_data = read_pcm_data(currentFileToMerge); 

                                    if (pcm_data) {
                                        insert(pcm_data); 
                                        TraceLog(LOG_DEBUG, "Added %s to merge list.", GetFileName(currentFileToMerge));
                                        filesAddedToMerge++;
                                    } else {
                                        TraceLog(LOG_WARNING, "Failed to read PCM data for merging: %s", GetFileName(currentFileToMerge));
                                    }
                                } else {
                                     TraceLog(LOG_WARNING, "Skipping non-WAV or non-existent file for merging: %s", GetFileName(currentFileToMerge));
                                }
                           }

                           if (filesAddedToMerge > 0) {
                               
                               write_pcm_data(); 
                               
                               char intendedMergedOutputFile[MAX_FILEPATH_SIZE];
                               snprintf(intendedMergedOutputFile, MAX_FILEPATH_SIZE, "merged_audio_%d.wav", merge_count);
                               TraceLog(LOG_INFO, "Called write_pcm_data(). Intended output: %s", intendedMergedOutputFile);

                               merge_count++; // Increment merge counter

                               if (filePathCounter < MAX_FILEPATH_RECORDED) {
                                   TextCopy(filePaths[filePathCounter], intendedMergedOutputFile);
                                   filePathCounter++;
                               } else {
                                   TraceLog(LOG_WARNING, "Max files reached, cannot add merged file to list.");
                               }

                           } else {
                               TraceLog(LOG_WARNING, "No valid files were added to the merge list.");
                           }
                           free_list(); 
                       } else {
                           TraceLog(LOG_WARNING, "Merge clicked, but no files dropped to merge.");
                       }
                       // isActivedrop = false;
                       // isfilter = false;
                    }
                    
                    else if (CheckCollisionPointRec(mousePos2, lowfileBounds)) {
                    
                        isfilter = !isfilter; 
                        if (isfilter) {
                            isActivedrop = false; 
                            TraceLog(LOG_INFO, "Filter mode toggled: ON");
                        } else {
                            TraceLog(LOG_INFO, "Filter mode toggled: OFF");
                        }
                    }
                    // 4. Reverse Action
                    else if (CheckCollisionPointRec(mousePos2, reversefileBounds)) {
                       if (selectedFileIndex != -1) {
                           const char* inputFile = filePaths[selectedFileIndex];
                            // Check file validity
                           if (FileExists(inputFile) && IsFileExtension(inputFile, ".wav")) {
                               char outputFile[MAX_FILEPATH_SIZE];
                               snprintf(outputFile, MAX_FILEPATH_SIZE, "%s_reversed_%d.wav", GetFileNameWithoutExt(inputFile), reverse_count);

                               reverse(inputFile, outputFile); 
                        
                               TraceLog(LOG_INFO, "Reversed file: %s -> %s", GetFileName(inputFile), outputFile);
                               reverse_count++;

                               // Optional: Add reversed file to list
                               if (filePathCounter < MAX_FILEPATH_RECORDED) {
                                   TextCopy(filePaths[filePathCounter], outputFile);
                                   filePathCounter++;
                               } else {
                                   TraceLog(LOG_WARNING, "Max files reached, cannot add reversed file to list.");
                               }
                           } else {
                                TraceLog(LOG_WARNING, "Cannot reverse: Selected item is not a valid WAV file or does not exist.");
                           }
                       } else {
                           TraceLog(LOG_WARNING, "Reverse clicked, but no file selected.");
                       }
                       // isActivedrop = false;
                       // isfilter = false;
                    }
                } 


                
                if (isActivedrop) {
                    
                    for (int i = 0; i < filePathCounter; i++)
                    {
                        
                        Rectangle fileEntry = {dock_width, 85.0f + 40.0f * i, 350.0f, 40.0f}; // Panel starts at x=dock_width
                        if (CheckCollisionPointRec(mousePos2, fileEntry) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            selectedFileIndex = i;
                            TraceLog(LOG_INFO, "Selected file index: %d (%s)", selectedFileIndex, GetFileName(filePaths[selectedFileIndex]));
                            // isActivedrop = false; 
                        }
                    }
                } 

                if (isfilter && selectedFileIndex != -1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    const char* inputFile = filePaths[selectedFileIndex];
                    char outputFile[MAX_FILEPATH_SIZE];
                    bool filterApplied = false; 

                
                    if (!FileExists(inputFile) || !IsFileExtension(inputFile, ".wav")) {
                        TraceLog(LOG_WARNING, "Cannot apply filter: Selected item is not a valid WAV file or does not exist.");
                    } else {
                        
                        if (CheckCollisionPointRec(mousePos2, level1Bounds)) {
                            snprintf(outputFile, MAX_FILEPATH_SIZE, "%s_lowpass_%d.wav", GetFileNameWithoutExt(inputFile), lowpass_count);
                            low_filter(inputFile, outputFile); // Call filter function
                            TraceLog(LOG_INFO, "Applied low-pass filter (Level 1) to: %s -> %s", GetFileName(inputFile), outputFile);
                            filterApplied = true;
                        }
                        else if (CheckCollisionPointRec(mousePos2, level2Bounds)) {
                            snprintf(outputFile, MAX_FILEPATH_SIZE, "%s_butterworth1_%d.wav", GetFileNameWithoutExt(inputFile), lowpass_count);
                            butterworth_low_filter(inputFile, outputFile); // Call filter function
                            TraceLog(LOG_INFO, "Applied low-pass filter (Level 2) to: %s -> %s", GetFileName(inputFile), outputFile);
                            filterApplied = true;
                        }
                        else if (CheckCollisionPointRec(mousePos2, level3Bounds)) {
                            snprintf(outputFile, MAX_FILEPATH_SIZE, "%s_butterworth3_%d.wav", GetFileNameWithoutExt(inputFile), lowpass_count);
                            butterworth_filter_3rd_order(inputFile, outputFile); // Call filter function
                            TraceLog(LOG_INFO, "Applied low-pass filter (Level 3) to: %s -> %s", GetFileName(inputFile), outputFile);
                            filterApplied = true;
                        }
                        else if (CheckCollisionPointRec(mousePos2, level4Bounds)) {
                            snprintf(outputFile, MAX_FILEPATH_SIZE, "%s_butterworth4_%d.wav", GetFileNameWithoutExt(inputFile), lowpass_count);
                            butterworth_filter_4th_order(inputFile, outputFile); // Call filter function
                            TraceLog(LOG_INFO, "Applied low-pass filter (Level 4) to: %s -> %s", GetFileName(inputFile), outputFile);
                            filterApplied = true;
                        }

                
                        if (filterApplied) {
                            lowpass_count++; 
                            isfilter = false; 

                            if (filePathCounter < MAX_FILEPATH_RECORDED) {
                                TextCopy(filePaths[filePathCounter], outputFile);
                                filePathCounter++;
                            } else {
                                TraceLog(LOG_WARNING, "Max files reached, cannot add filtered file to list.");
                            }
                            
                            // selectedFileIndex = filePathCounter - 1;
                        }
                    } 
                } 
                break; // End case Static

            default:
                 TraceLog(LOG_ERROR, "Invalid screen state encountered!");
                 currentScreen = Load; 
                break;
        }

        BeginDrawing();
        ClearBackground(BLACK); 

        switch(currentScreen)
        {
            case Load:
            {
                
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
                
                 const char* realHelp = "Click button to Record/Stop. [Enter] for Static, [Space] for Load.";
                 int fontSizeHelp = 18;
                 DrawText(realHelp, 10, SCREEN_HEIGHT - fontSizeHelp - 10, fontSizeHelp, GRAY);
            } break; 

            case Static:
            {
            
                Color softgray = (Color){30, 34, 42, 100};
                Color panelColor = softgray; 
                Color hoverColor = Fade(YELLOW, 0.3f);
                Color selectedColor = Fade(BLUE, 0.3f); 
                Color fileEvenColor = Fade(DARKGRAY, 0.3f);
                Color fileOddColor = Fade(DARKGRAY, 0.1f);
                Color textColor = WHITE;
                Color textDimColor = GRAY;


            
                Rectangle dock = { 0, 0, dock_width, (float)SCREEN_HEIGHT };
                DrawRectangleRec(dock, Fade(softgray, 0.5f));

                 float dock_draw_y = padding; 
                 DrawTexture(texture3, (int)((dock_width - (float)texture3.width) / 2.0f), (int)dock_draw_y, WHITE);
                 dock_draw_y += (float)texture3.height + padding;
                 DrawTexture(texture4, (int)((dock_width - (float)texture4.width) / 2.0f), (int)dock_draw_y, WHITE);
                 dock_draw_y += (float)texture4.height + padding;
                 // Highlight filter icon if filter mode is active
                 DrawTexture(texture5, (int)((dock_width - (float)texture5.width) / 2.0f), (int)dock_draw_y, isfilter ? SKYBLUE : WHITE);
                 dock_draw_y += (float)texture5.height + padding;
                 DrawTexture(texture6, (int)((dock_width - (float)texture6.width) / 2.0f), (int)dock_draw_y, WHITE);


                
                 Rectangle mainArea = { dock_width, 0, SCREEN_WIDTH - dock_width, SCREEN_HEIGHT };

                
                 if (isActivedrop)
                 {
                     Rectangle filePanel = { dock_width, 0, 350, (float)SCREEN_HEIGHT }; 
                     DrawRectangleRec(filePanel, panelColor); // Use original panel color
                     DrawText("Dropped files:", (int)(dock_width + 10), 40, 20, DARKGRAY); 

            
                     for (int i = 0; i < filePathCounter; i++)
                     {
                         Rectangle fileEntry = {dock_width, 85.0f + 40.0f * i, 350.0f, 40.0f};

                         Vector2 mousePosFiles = GetMousePosition(); // Use current mouse pos
                         bool isHovered = CheckCollisionPointRec(mousePosFiles, fileEntry);

                         // Draw background based on state (Original style)
                         if (selectedFileIndex == i) {
                             DrawRectangleRec(fileEntry, selectedColor);
                         } else if (isHovered) {
                             DrawRectangleRec(fileEntry, hoverColor);
                         } else if (i % 2 == 0) {
                             DrawRectangleRec(fileEntry, fileEvenColor);
                         } else {
                             DrawRectangleRec(fileEntry, fileOddColor);
                         }

                         
                         DrawText(GetFileName(filePaths[i]), (int)(fileEntry.x + 10), (int)(fileEntry.y + 10), 20, textColor);
                     }

                    
                     Rectangle waveformArea = { filePanel.x + filePanel.width, 0, SCREEN_WIDTH - (filePanel.x + filePanel.width), SCREEN_HEIGHT};
                     if (selectedFileIndex != -1) {
                         const char* currentSelectedFilePath = filePaths[selectedFileIndex];
                         if (FileExists(currentSelectedFilePath) && IsFileExtension(currentSelectedFilePath, ".wav")) {
                             read_wav_header(currentSelectedFilePath); 
                             if (header.subchunk2size > 0 && header.bitspersample == 16) { 
                                 int num_samples = header.subchunk2size / (header.numchannels * (header.bitspersample / 8)); 
                                
                                 draw_waveform(currentSelectedFilePath, num_samples); 
                                
                             } else {
                                 
                                 DrawText("Cannot display waveform for this file format", (int)(waveformArea.x + 20), (int)(waveformArea.y + SCREEN_HEIGHT / 2), 20, RED);
                             }
                         } else {
                              
                              DrawText("Selected file is not a WAV or does not exist.", (int)(waveformArea.x + 20), (int)(waveformArea.y + SCREEN_HEIGHT / 2), 20, RED);
                         }
                     } else {
                        
                         const char *message1 = "Select a file";
                         int fontSize1 = 30;
                         int textWidth1 = MeasureText(message1, fontSize1);
                         DrawText(message1, (int)(waveformArea.x + (waveformArea.width - textWidth1)/2), (int)(waveformArea.y + SCREEN_HEIGHT/2 - fontSize1/2), fontSize1, textDimColor);
                     }
                 }
        
                 else if (isfilter)
                 {
                     
                    DrawTexture(texture7, (int)level1Bounds.x, (int)level1Bounds.y, CheckCollisionPointRec(GetMousePosition(), level1Bounds) ? YELLOW: WHITE);
                    DrawTexture(texture8, (int)level2Bounds.x, (int)level2Bounds.y, CheckCollisionPointRec(GetMousePosition(), level2Bounds) ? YELLOW: WHITE);
                    DrawTexture(texture9, (int)level3Bounds.x, (int)level3Bounds.y, CheckCollisionPointRec(GetMousePosition(), level3Bounds) ? YELLOW: WHITE);
                    DrawTexture(texture10, (int)level4Bounds.x, (int)level4Bounds.y, CheckCollisionPointRec(GetMousePosition(), level4Bounds) ? YELLOW: WHITE);

                     
                     const char *message7;
                     int fontSize7 = 30;
                     if (selectedFileIndex == -1) {
                         message7 = "Select a file first (click file icon), then click level";
                     } else {
                         
                         message7 = TextFormat("Apply Filter to: %s", GetFileName(filePaths[selectedFileIndex]));
                     }
                     int textWidth7 = MeasureText(message7, fontSize7);
                     
                     DrawText(message7, (int)(mainArea.x + (mainArea.width - textWidth7)/2), (int)(mainArea.y + 50), fontSize7, textDimColor); // Position near top
                 }
                 
                 else
                 {
                     const char *message;
                     int fontSize = 30;
                     if (filePathCounter == 0) {
                          message = "Drop your files to this window!"; 
                     } else {
                          message = "Drop more files or click icon"; 
                          
                          // selectedFileIndex = -1; // This was in the original logic block here
                     }
                     int textWidth = MeasureText(message, fontSize);
                     DrawText(message, (int)(mainArea.x + (mainArea.width - textWidth)/2), (int)(mainArea.y + SCREEN_HEIGHT/2 - fontSize/2), fontSize, textDimColor); // Centered
                 }
                 

            } break; 

            default: break;
        }

        
        DrawFPS(SCREEN_WIDTH - 90, 10);

        EndDrawing();
        
    } 


     if (isActiverec) StopAudioRecording(); 

    UnloadTexture(texture1);
    UnloadTexture(texture3);
    UnloadTexture(texture4);
    UnloadTexture(texture5);
    UnloadTexture(texture6);
    UnloadTexture(texture7);
    UnloadTexture(texture8);
    UnloadTexture(texture9);
    UnloadTexture(texture10);

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
         if (filePaths[i] != NULL) RL_FREE(filePaths[i]);
    }


    free_list(); 

    CloseAudioDevice(); 
    CloseWindow();      

    return 0;
}