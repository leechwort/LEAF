/*
  ==============================================================================

    basic-oscillators.c
    Created: 5 Mar 2020 2:56:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "basic-oscillators.h"
#include "samplewriter.h"

#define AUDIO_BUFFER_SIZE (44100 * 5)
#define SAMPLERATE 44100
float audioBuffer[AUDIO_BUFFER_SIZE];

float exampleRandom()
{
    return ((float)rand() / (float)(RAND_MAX));
}

int main()
{
    LEAF leaf;

    LEAF_init(&leaf, SAMPLERATE, mempool, AUDIO_BUFFER_SIZE, &exampleRandom);

    tCycle_init(&cycle, &leaf);
    tCycle_setFreq(&cycle, 220);

    for (int i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        audioBuffer[i] = tCycle_tick(&cycle);
    }

    saveSample(audioBuffer, AUDIO_BUFFER_SIZE, SAMPLERATE);
    return 0;
}
