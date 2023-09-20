/*
  ==============================================================================

    basic-oscillators.c
    Created: 5 Mar 2020 2:56:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "basic-oscillators.h"
#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

#define AUDIO_BUFFER_SIZE (44100*5)
#define SAMPLERATE 44100
float audioBuffer[AUDIO_BUFFER_SIZE];

float exampleRandom()
{
    return ((float)rand()/(float)(RAND_MAX));
}

int main()
{

    const char* filename = "output.wav";

    // Open the audio file for writing
    SF_INFO sfinfo;
    sfinfo.samplerate = SAMPLERATE;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    SNDFILE* outfile = sf_open(filename, SFM_WRITE, &sfinfo);
    if (!outfile) {
        printf("Error opening audio file for writing\n");
        return 1;
    }
    
    LEAF leaf;
    
    LEAF_init(&leaf, SAMPLERATE, mempool, AUDIO_BUFFER_SIZE, &exampleRandom);
    
    tCycle_init(&cycle, &leaf);
    tCycle_setFreq(&cycle, 220);
    
    for (int i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        audioBuffer[i] = tCycle_tick(&cycle);
    }
    
    // Write the audio data to the file
    sf_writef_float(outfile, audioBuffer, AUDIO_BUFFER_SIZE);

    // Close the audio file
    sf_close(outfile);
    return 0;
}
