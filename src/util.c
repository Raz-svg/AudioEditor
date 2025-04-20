#include "util.h"
WAVHeader header;


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

    printf("Audio Format: %d\n", header.audioformat);
    printf("Channels: %d\n", header.numchannels);
    printf("Sample Rate: %d Hz\n", header.samplerate);
    printf("Bit Depth: %d-bit\n", header.bitspersample);
    printf("Data Size: %u bytes\n", header.chunksize);
    printf("Subchunk ID: %s\n", header.subchunk1id);
    printf("Subchunk1 Size: %d\n", header.subchunk1size);

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

void write_pcm_data() {
    int total_samples = 0;
    Node* temp = head;
    while (temp != NULL) {
        total_samples += header.subchunk2size;
        temp = temp->next;
    }

    WAVHeader new_header = header;
    new_header.subchunk2size = total_samples;
    new_header.chunksize = 36 + total_samples;

    FILE* file = fopen("output.wav", "wb");
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
