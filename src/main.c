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

struct node{

    int16_t *pcm;
    struct node *next;
};
struct node* head=NULL;

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

    printf("Audio Format: %d\n", header.audioformat);// generally pcm is 1
    printf("Channels: %d\n", header.numchannels);
    printf("Sample Rate: %d Hz\n", header.samplerate);
    printf("Bit Depth: %d-bit\n", header.bitspersample);
    printf("Data Size: %u bytes\n", header.chunksize);
    printf("subchunlid: %s\n",header.subchunk1id);
    printf("subchunk1size: %d\n",header.subchunk1size);

    fclose(file);
}

// reading raw pcm data
int16_t *read_pcm_data(const char* filename) {
    FILE* file = fopen(filename, "rb");

    // moves the pointer offset bytes from the origin
    fseek(file, 44, SEEK_SET);// skip the header i.e 44 bytes

    // read the data  
    int16_t* data = (int16_t*)malloc(header.subchunk2size);
    fread(data, sizeof(int16_t), header.subchunk2size/sizeof(int16_t), file);

    // do something with the data
    fclose(file);
    return data;
}

struct node *insert(int16_t *data){
    struct node *temp=(struct node*)malloc(sizeof(struct node));
    temp->pcm=data;
    temp->next=NULL;
    if(head == NULL){
        head=temp;
    }
    else{
        struct node* temp1=head;
        while(temp1->next!=NULL){
            temp1=temp1->next;
        }
        temp1->next=temp;

    }
    return head;

}


int main() {

    // this also for more than 2 wav files
    read_wav_header("sampl.wav");
    read_wav_header("second.wav");
    int16_t* v=read_pcm_data("sampl.wav");
    int16_t* b=read_pcm_data("second.wav");
   /* for(int i=0;i<1000;i++)
    *{
    *    printf("%d\n",v[i]);
    *}
    */

    /// need to make for more than two wav files
    insert(v);
    insert(b);

      // Writing to a new file
      FILE* file = fopen("connected.wav", "wb");
      if (file == NULL) {
          printf("Error creating file\n");
          return 1;
      }
      

      // change this ---> for random  size
      // Update header for new file
      WAVHeader new_header = header;
      new_header.subchunk2size *= 2; // Double the data size
      new_header.chunksize = 36 + new_header.subchunk2size;
      
      // Write updated header
      fwrite(&new_header, sizeof(WAVHeader), 1, file);
      
      // Write PCM data from linked list
      struct node* temp = head;
      while (temp != NULL) {
          fwrite(temp->pcm, sizeof(int16_t), header.subchunk2size / sizeof(int16_t), file);
          temp = temp->next;
      }
      
      fclose(file);
      printf("WAV files successfully merged into connected.wav\n");
      
      return 0;
    



    return 0;
}