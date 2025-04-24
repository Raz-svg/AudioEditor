#include "util.h"
WAVHeader header;

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1000

Color softYellow = (Color){255, 255, 153, 100};  
int factor = 2;
Node* head = NULL;

void read_wav_header(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    fread(&header, sizeof(WAVHeader), 1, file);
    
    if (strncmp(header.chunkid, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0) {
        printf("Not a valid WAV file: %s\n", filename);
        fclose(file);
        return;// Hello
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
//Enters the PCM data at the end of the singly linked list
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

    DrawLine(200, centerY, SCREEN_WIDTH, centerY, BLACK);    

    read_wav_header(filepath);
    int16_t *pcm = read_pcm_data(filepath);
    if (!pcm) return;

    for (int i = 0; i < num_samples; i++) {
        int x1 = i * SCREEN_WIDTH / num_samples+200;
        int y1 = centerY - (pcm[i] / scale);
        int x2 = x1;
        int y2 = centerY;
        DrawLine(x1, y1, x2, y2, softYellow);
    }

    free(pcm);  
}
void zoom(int *num_samples) {
    *num_samples = (*num_samples) / factor;
}
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
    
    