#ifndef WAV_UTILS_H
#define WAV_UTILS_H

#include "raylib.h"
#include "miniaudio.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include<math.h>


typedef struct {
    char chunkid[4];
    int32_t chunksize;
    char format[4];
    char subchunk1id[4];
    int32_t subchunk1size;
    int16_t audioformat;
    int16_t numchannels;
    int32_t samplerate;
    int32_t byterate;
    int16_t blockalign;
    int16_t bitspersample;
    char subchunk2id[4];
    int32_t subchunk2size;
} WAVHeader;

typedef struct node{
    int16_t *pcm;
    struct node *next;
} Node;

typedef struct{
    Node *head;
    int16_t *pcm;
} Track;

typedef struct {
    Rectangle rect;
    bool dragging;
    Vector2 offset;
    Color color;
} DraggableBox;

typedef enum{
    Load=0,
    Real,
    Static,
}Screen;

void read_wav_header(const char* filename);

int16_t* read_pcm_data(const char* filename);

Node* insert(int16_t *data);

void write_pcm_data(char* filename);

void free_list();

void reverse(const char *input_file, const char *output_file);

void draw_waveform(const char *filepath, int num_samples) ;

void zoom(int *num_samples) ;
void low_filter(const char *input_file, const char *output_file2);
void butterworth_low_filter(const char *input_file, const char *output_file);
void butterworth_filter_4th_order(const char *input_file, const char *output_file);
void butterworth_filter_3rd_order(const char *input_file, const char *output_file);

void StartAudioRecording(void);
void StopAudioRecording(char* filename);

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
extern WAVHeader header;
extern Node* head;

#endif
