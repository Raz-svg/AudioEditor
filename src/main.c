#include<stdio.h>
#include<stdlib.h>

// testing

typedef struct{

    //riff header
    char chunkid[4]="RIFF";
    int chunksize;
    char format[4]="WAVE";
    //fmt(format) subchunk
    char subchunk1id[4]="fmt ";
    int subchunk1size=16;
    //data subchunk
    char subchunk2id[4]="data";
    int subchunk2size;
    
}header;

int main(){

}