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
    int32_t subchunk2size;

} WAVHeader;

void read_wav_header(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    WAVHeader header;
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

int main() {
    read_wav_header("sample.wav");
    return 0;
}