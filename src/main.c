#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // For fixed-size integers

typedef struct {
    // RIFF Header
    char chunkid[4];
    int32_t chunksize;
    char format[4];
    // fmt Subchunk
    char subchunk1id[4];
    int32_t subchunk1size;
    int16_t audioformat;
    int16_t numchannels;// streo = 2, mono = 1
    int32_t samplerate;
    int32_t byterate;
    int16_t blockalign;
    int16_t bitspersample;
    // data Subchunk
    char subchunk2id[4];
    int32_t subchunk2size; // actual audio data size

} WAVHeader;

WAVHeader header;


// mainly for debugging
void read_wav_header(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    fread(&header, sizeof(WAVHeader), 1, file);
    
    if (strncmp(header.chunkid, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0) {
        printf("Not a valid WAV file!\n");
        fclose(file);
        return;
    }

    printf("Audio Format: %d\n", header.audioformat);// generallu pcm is 1
    printf("Channels: %d\n", header.numchannels);
    printf("Sample Rate: %d Hz\n", header.samplerate);
    printf("Bit Depth: %d-bit\n", header.bitspersample);
    printf("Data Size: %u bytes\n", header.chunksize);

    fclose(file);
}

// reading raw pcm data
int16_t *read_pcm_data(const char* filename) {
    FILE* file = fopen(filename, "rb");

    // moves the pointer offset bytes from the origin
    fseek(file, 44, SEEK_SET);// skip the header i.e 44 bytes

    // read the data
    int16_t* data = (int16_t*)malloc(header.subchunk2size);
    fread(data, header.subchunk2size, 1, file);

    // do something with the data
    fclose(file);
    return data;
}


int main() {
    
    read_wav_header("sample.wav");
    int16_t* v=read_pcm_data("sample.wav");
    for(int i=0;i<100;i++)
    {
        printf("%d\n",*(v+i));
    }
    
    return 0;
}