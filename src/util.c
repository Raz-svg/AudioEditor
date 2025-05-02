#include "util.h"
#include "miniaudio.h" 
#include <stdbool.h> 

WAVHeader header;
ma_device device;
bool isRecording = false;
#define MAX_FILEPATH_RECORDED   4096
// #define MAX_FILEPATH_SIZE       
#define SCREEN_WIDTH 1600          
#define SCREEN_HEIGHT 1000         


Color softYellow = (Color){255, 255, 153, 100};  


int factor = 2; 
Node* head = NULL;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);


void read_wav_header(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        
    }

    fread(&header, sizeof(WAVHeader), 1, file);
    
    if (strncmp(header.chunkid, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0) {
        printf("Not a valid WAV file: %s\n", filename);
        fclose(file);
    
    }

    /*printf("Audio Format: %d\n", header.audioformat);
   printf("Audio Format: %d\n", header.audioformat);
    printf("Channels: %d\n", header.numchannels);
    printf("Sample Rate: %d Hz\n", header.samplerate);
    printf("Bit Depth: %d-bit\n", header.bitspersample);
    printf("Data Size: %u bytes\n", header.chunksize);
    printf("Subchunk ID: %s\n", header.subchunk1id);
    printf("Subchunk1 Size: %d\n", header.subchunk1size);
    printf("Subchunk1 Size: %d\n", header.subchunk1size);*/

    fclose(file);
}

int16_t* read_pcm_data(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    fseek(file, 44, SEEK_SET);
    int16_t* data = (int16_t*)malloc(header.subchunk2size);
    if (!data) {
        printf("Memory allocation failed!\n");
        fclose(file);
        return NULL;
    }

    fread(data, sizeof(int16_t), header.subchunk2size / sizeof(int16_t), file);
    fclose(file);
    return data;
}

Node* insert(int16_t *data) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->pcm = data;
    temp->next = NULL;
    if (head == NULL) {
        head = temp;
    } else {
        Node* temp1 = head;
        while (temp1->next != NULL) {
            temp1 = temp1->next;
        }
        temp1->next = temp;
    }
    return head;
}

void free_list() {
    Node* temp = head;
    while (temp != NULL) {
        Node* next = temp->next;
        free(temp->pcm);
        free(temp);
        temp = next;
    }
    head = NULL;
}

void write_pcm_data(char* filename) {
    int total_samples = 0;
    Node* temp = head;
    while (temp != NULL) {
        total_samples += header.subchunk2size;
        temp = temp->next;
    }

    WAVHeader new_header = header;
    new_header.subchunk2size = total_samples;
    new_header.chunksize = 36 + total_samples;

    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error opening file for writing\n");
        return;
    }

    fwrite(&new_header, sizeof(WAVHeader), 1, file);

    temp = head;
    while (temp != NULL) {
        fwrite(temp->pcm, sizeof(int16_t), header.subchunk2size / sizeof(int16_t), file);
        temp = temp->next;
    }

    fclose(file);
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

void draw_waveform(const char *filepath, int num_samples) {

    int centerY = (SCREEN_HEIGHT / 2);
    int scale = 100;

    DrawLine(400, centerY, SCREEN_WIDTH, centerY, BLACK);

    int16_t *pcm = read_pcm_data(filepath); 
    if (!pcm) {
        printf("Failed to read PCM data for waveform drawing.\n");
        return;
    }

    if (header.subchunk2size <= 0 || header.bitspersample <= 0) {
         printf("Invalid WAV header data for waveform drawing.\n");
         free(pcm);
         return;
    }
    int bytes_per_sample = header.bitspersample / 8;
    if (bytes_per_sample <= 0) bytes_per_sample = 2; 
    int total_pcm_samples = header.subchunk2size / bytes_per_sample;

    if (num_samples <= 0 || num_samples > total_pcm_samples) {
        num_samples = total_pcm_samples; 
    }
     if (num_samples <= 0) { 
         printf("No samples to draw.\n");
         free(pcm);
         return;
     }


    int waveform_width = SCREEN_WIDTH - 400; 

    for (int i = 0; i < waveform_width; i++) {
        
        int sample_index = (int)(((float)i / waveform_width) * num_samples);

         if (sample_index >= total_pcm_samples) sample_index = total_pcm_samples - 1;
         if (sample_index < 0) sample_index = 0;


        int x = 400 + i;
        int y = centerY - (pcm[sample_index] / scale);
        if (y < 0) y = 0;
        if (y > SCREEN_HEIGHT) y = SCREEN_HEIGHT;

        DrawLine(x, centerY, x, y, softYellow);
    }

    free(pcm);
}

// Update zoom function definition to accept factor
/*void zoom(int *num_samples, float factor) { // Match prototype in util.h
    if (factor > 0) {
        *num_samples = (int)(*num_samples / factor);
        if (*num_samples < 1) *num_samples = 1;
    }
}*/

void low_filter(const char *input_file, const char *output_file2)
{
    read_wav_header(input_file);
    int16_t *data = read_pcm_data(input_file);
    if (!data) return;
    int num_samples = header.subchunk2size / sizeof(int16_t);
    int16_t *filtered_data = (int16_t*)malloc(header.subchunk2size);
    if (!filtered_data) {
        printf("Memory allocation failed for filtered data!\n");
        free(data);
        return;
    }
    const int window_size = 5;
    const float scale = 1.0f / window_size;
    for (int i = 0; i < window_size/2; i++) {
        filtered_data[i] = data[i];
    }
    for (int i = window_size/2; i < num_samples - window_size/2; i++) {
        int32_t sum = 0;
        for (int j = -window_size/2; j <= window_size/2; j++) {
            sum += data[i + j];
        }
        filtered_data[i] = (int16_t)(sum * scale);
    }
    for (int i = num_samples - window_size/2; i < num_samples; i++) {
        filtered_data[i] = data[i];
    }
    FILE *out = fopen(output_file2, "wb");
    if (!out) {
        printf("Error opening output file: %s\n", output_file2);
        free(data);
        free(filtered_data);
        return;
    }
    fwrite(&header, sizeof(WAVHeader), 1, out);
    fwrite(filtered_data, sizeof(int16_t), header.subchunk2size / sizeof(int16_t), out);
    fclose(out);
    free(filtered_data);
    free(data);
}   

void butterworth_low_filter(const char *input_file, const char *output_file) {
    read_wav_header(input_file);
    int16_t *data = read_pcm_data(input_file);
    if (!data) return;

    int num_samples = header.subchunk2size / sizeof(int16_t);
    int16_t *filtered_data = (int16_t*)malloc(header.subchunk2size);
    if (!filtered_data) {
        printf("Memory allocation failed for filtered data!\n");
        free(data);
        return;
    }
    const float a0 = 0.0201;
    const float a1 = 0.0402;
    const float a2 = 0.0201;
    const float b1 = -1.5610;
    const float b2 = 0.6414;
    float x1 = 0, x2 = 0;  
    float y1 = 0, y2 = 0; 
    for (int i = 0; i < num_samples; i++) {
        float x0 = (float)data[i];
        float y0 = a0 * x0 + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
        filtered_data[i] = (int16_t)y0;
        x2 = x1;
        x1 = x0;
        y2 = y1;
        y1 = y0;
    }
    FILE *out = fopen(output_file, "wb");
    if (!out) {
        printf("Error opening output file: %s\n", output_file);
        free(data);
        free(filtered_data);
        return;
    }
    fwrite(&header, sizeof(WAVHeader), 1, out);
    fwrite(filtered_data, sizeof(int16_t), header.subchunk2size / sizeof(int16_t), out);
    fclose(out);
    free(filtered_data);
    free(data);
}
void butterworth_filter_3rd_order(const char *input_file, const char *output_file) {
    read_wav_header(input_file);
    int16_t *data = read_pcm_data(input_file);
    if (!data) return;
    int num_samples = header.subchunk2size / sizeof(int16_t);
    int16_t *filtered_data = (int16_t*)malloc(header.subchunk2size);
    if (!filtered_data) {
        printf("Memory allocation failed for filtered data!\n");
        free(data);
        return;
    }
    // First order section coefficients
    const float a0_1 = 0.0675f;
    const float a1_1 = 0.0675f;
    const float b1_1 = -0.8650f;
    // Second order section coefficients
    const float a0_2 = 0.0088f;
    const float a1_2 = 0.0176f;
    const float a2_2 = 0.0088f;
    const float b1_2 = -1.7347f;
    const float b2_2 = 0.7699f;
    // Delay buffers
    float x1_1 = 0, y1_1 = 0;                    // First order section
    float x1_2 = 0, x2_2 = 0, y1_2 = 0, y2_2 = 0;  // Second order section
    // Apply cascaded filters
    for (int i = 0; i < num_samples; i++) {
        // First order section
        float x0_1 = (float)data[i];
        float y0_1 = a0_1 * x0_1 + a1_1 * x1_1 - b1_1 * y1_1;
        // Update first order delays
        x1_1 = x0_1;
        y1_1 = y0_1;
        // Second order section
        float x0_2 = y0_1;  // Input is output from first section
        float y0_2 = a0_2 * x0_2 + a1_2 * x1_2 + a2_2 * x2_2 - b1_2 * y1_2 - b2_2 * y2_2;
        // Update second order delays
        x2_2 = x1_2;
        x1_2 = x0_2;
        y2_2 = y1_2;
        y1_2 = y0_2;
        // Scale and store the final output
        filtered_data[i] = (int16_t)(y0_2 * 0.85f);  // Scaling factor
    }
    // Write filtered data to output file
    FILE *out = fopen(output_file, "wb");
    if (!out) {
        printf("Error opening output file: %s\n", output_file);
        free(data);
        free(filtered_data);
        return;
    }
    fwrite(&header, sizeof(WAVHeader), 1, out);
    fwrite(filtered_data, sizeof(int16_t), header.subchunk2size / sizeof(int16_t), out);
    fclose(out);
    free(filtered_data);
    free(data);
}
void butterworth_filter_4th_order(const char *input_file, const char *output_file) {
    read_wav_header(input_file);
    int16_t *data = read_pcm_data(input_file);
    if (!data) return;

    int num_samples = header.subchunk2size / sizeof(int16_t);
    int16_t *filtered_data = (int16_t*)malloc(header.subchunk2size);
    if (!filtered_data) {
        printf("Memory allocation failed for filtered data!\n");
        free(data);
        return;
    }
    // First section
    const float a0_1 = 0.0048f;
    const float a1_1 = 0.0096f;
    const float a2_1 = 0.0048f;
    const float b1_1 = -1.8904f;
    const float b2_1 = 0.8096f;
    // Second section
    const float a0_2 = 1.0f;
    const float a1_2 = 2.0f;
    const float a2_2 = 1.0f;
    const float b1_2 = -1.9159f;
    const float b2_2 = 0.9355f;
    // Delay buffers for both sections
    float x1_1 = 0, x2_1 = 0, y1_1 = 0, y2_1 = 0;  // First section
    float x1_2 = 0, x2_2 = 0, y1_2 = 0, y2_2 = 0;  // Second section
    // Apply cascaded filters
    for (int i = 0; i < num_samples; i++) {
        // First section
        float x0_1 = (float)data[i];
        float y0_1 = a0_1*x0_1 + a1_1*x1_1 + a2_1*x2_1 - b1_1*y1_1 - b2_1*y2_1;
        // Update first section delays
        x2_1 = x1_1;
        x1_1 = x0_1;
        y2_1 = y1_1;
        y1_1 = y0_1;
        // Second section
        float x0_2 = y0_1;  // Input is output from first section
        float y0_2 = a0_2*x0_2 + a1_2*x1_2 + a2_2*x2_2 - b1_2*y1_2 - b2_2*y2_2;
        // Update second section delays
        x2_2 = x1_2;
        x1_2 = x0_2;
        y2_2 = y1_2;
        y1_2 = y0_2;
        // Scale and store the final output
        filtered_data[i] = (int16_t)(y0_2 * 0.0001f);  // Scaling factor to prevent overflow
    }
    // Write filtered data to output file
    FILE *out = fopen(output_file, "wb");
    if (!out) {
        printf("Error opening output file: %s\n", output_file);
        free(data);
        free(filtered_data);
        return;
    }
    fwrite(&header, sizeof(WAVHeader), 1, out);
    fwrite(filtered_data, sizeof(int16_t), num_samples, out);
    fclose(out);
    free(filtered_data);
    free(data);
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    (void)pDevice;
    (void)pOutput; 

    if (pInput == NULL) return;

    FILE* file = fopen("real_time_recording.raw", "ab");
    if (file) {
        fwrite(pInput, sizeof(int16_t), frameCount, file);
        fclose(file);
    }
}



void StartAudioRecording()
{
    ma_device_config deviceConfig;

    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format   = ma_format_s16;
    deviceConfig.capture.channels = 1;
    deviceConfig.sampleRate       = 44100;
    deviceConfig.dataCallback     = data_callback;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to initialize capture device.\n");
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start capture device.\n");
        ma_device_uninit(&device);
        return;
    }

    isRecording = true;
}

void StopAudioRecording(char* recordedFileName)
{
    if (isRecording) {
        ma_device_uninit(&device);
        isRecording = false;

        printf("Recording stopped.\n");

        
        FILE* raw = fopen("real_time_recording.raw", "rb");
        if (!raw) {
            printf("Failed to open raw recording file.\n");
            return;
        }

        fseek(raw, 0, SEEK_END);
        int raw_size = ftell(raw);
        fseek(raw, 0, SEEK_SET);

        FILE* wav = fopen(recordedFileName, "wb");
        if (!wav) {
            printf("Failed to open WAV output file.\n");
            fclose(raw);
            return;
        }

        
        WAVHeader wavHeader;
        memcpy(wavHeader.chunkid, "RIFF", 4);
        wavHeader.chunksize = 36 + raw_size;
        memcpy(wavHeader.format, "WAVE", 4);
        memcpy(wavHeader.subchunk1id, "fmt ", 4);
        wavHeader.subchunk1size = 16;
        wavHeader.audioformat = 1;
        wavHeader.numchannels = 1;
        wavHeader.samplerate = 44100;
        wavHeader.byterate = 44100 * sizeof(int16_t);
        wavHeader.blockalign = sizeof(int16_t);
        wavHeader.bitspersample = 16;
        memcpy(wavHeader.subchunk2id, "data", 4);
        wavHeader.subchunk2size = raw_size;

        fwrite(&wavHeader, sizeof(WAVHeader), 1, wav);

        
        char* buffer = (char*)malloc(raw_size);
        fread(buffer, 1, raw_size, raw);
        fwrite(buffer, 1, raw_size, wav);

        free(buffer);
        fclose(raw);
        fclose(wav);

        remove("real_time_recording.raw"); 
    }
}

