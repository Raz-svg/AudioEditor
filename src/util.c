#include "util.h"
WAVHeader header;

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048
#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()

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

   /* printf("Audio Format: %d\n", header.audioformat);
    printf("Channels: %d\n", header.numchannels);
    printf("Sample Rate: %d Hz\n", header.samplerate);
    printf("Bit Depth: %d-bit\n", header.bitspersample);
    printf("Data Size: %u bytes\n", header.chunksize);
    printf("Subchunk ID: %s\n", header.subchunk1id);
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

void zoom(int *num_samples) {
    *num_samples = (*num_samples) / factor;
}