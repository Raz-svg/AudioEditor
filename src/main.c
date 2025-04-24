<<<<<<< HEAD
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

=======
#include "util.c" 

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048



int main() {

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Waveform");

    Screen currentScreen = Load;
    Image logo = LoadImage("Group 3 (1).png");
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
       
        Vector2 buttonCenter = { (SCREEN_WIDTH - texture1.width) / 2 + texture1.width / 2+20, (SCREEN_HEIGHT - texture1.height) / 6 + texture1.height / 2 };
        float buttonRadius = texture1.width+10 ;

        BeginDrawing();
        ClearBackground(BLACK);
       
        switch(currentScreen)
        {
            case Load:
                 DrawText("Real Screen", 10, 10, 20, WHITE);
             
                break;
            case Real:
                    float pulse = 1.0f + 0.1f * sin(GetTime() * 6.0f);
                    DrawCircleV(buttonCenter, buttonRadius * pulse, (Color){255, 0, 0, 80*pulse});
                    DrawTexture(texture1,(SCREEN_WIDTH - texture1.width) / 2, (SCREEN_HEIGHT - texture1.height) / 6,WHITE);
          
                break;
            case Static:
            
                 if (filePathCounter == 0) {
                     const char *message = "Drop your files to this window!";
                     int fontSize = 40;
                     int textWidth = MeasureText(message, fontSize);
                
                    DrawText(message, (SCREEN_WIDTH - textWidth)/2, SCREEN_HEIGHT/2, fontSize, WHITE);
                
                 } 
                 else {
                    DrawRectangle(0, 0, 200, SCREEN_HEIGHT, Fade(DARKGRAY, 0.5f));
                    DrawText("Dropped files:", 10, 40, 20, BLACK);
                    for (int i = 0; i < filePathCounter; i++) 
                    {
                    if (i % 2 == 0)
                        DrawRectangle(0, 85 + 40 * i, 200, 40, Fade(LIGHTGRAY, 0.5f));
                    else
                        DrawRectangle(0, 85 + 40 * i, 200, 40, Fade(LIGHTGRAY, 0.3f));
    
                    DrawText(filePaths[i], 10, 95 + 40 * i, 10, DARKGRAY);
    
                    read_wav_header(filePaths[i]);
                    int num_samples = header.subchunk2size / sizeof(int16_t);
                    draw_waveform(filePaths[i], num_samples);
                 }
    
                 DrawText("Drop new files...", 10, 110 + 40 * filePathCounter, 20, WHITE);
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
>>>>>>> d2a800c592f43e3d7ec5ac83638202a9978c2ac1
