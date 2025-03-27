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
    int16_t numchannels;
    int32_t samplerate;
    int32_t byterate;
    int16_t blockalign;
    int16_t bitspersample;

    // data Subchunk
    char subchunk2id[4];
    int32_t subchunk2size;
} header;

int main() {
    FILE *fp;
    header h;
}
