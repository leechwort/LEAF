 /*
  ==============================================================================
	Example of use of sawtooth generator and delay.

    sawtooth-sequence.c
    Created: 20 Sep 2023
    Author:  Artem Synytsyn

  ==============================================================================
*/
#include "../leaf/leaf.h"
#include <stdio.h>
#include <stdlib.h>
#include "samplewriter.h"


char mempool[1000000];

tSawtooth sawtooth;
tHermiteDelay delay;

#define AUDIO_BUFFER_SIZE (44100*5)
#define SAMPLERATE 44100
float audioBuffer[AUDIO_BUFFER_SIZE];

float exampleRandom()
{
    return ((float)rand()/(float)(RAND_MAX));
}

float sequencer(int tick)
{
    float sequence[] = {55, 0, 77.78, 0, 69.30}; // A1, D#2, C#2
    int timeline[] = {0, 4000, 6000, 10000, 12000, 16000, 18000}; // Timeline in ticks for each note
    
    for (size_t i = 1; i < (sizeof(timeline)/sizeof(timeline[0])); i++)
    {
        // Check what timeslot is corresponding and output specific note
        if ((tick > timeline[i - 1]) && (tick < timeline[i]))
        {
            // Do not output anything if frequency is zero
            if (sequence[i - 1] == 0.0)
                return 0;
            tSawtooth_setFreq(&sawtooth, sequence[i - 1]);
            return tSawtooth_tick(&sawtooth);
        }

    }
    return 0;
}

int main()
{
    LEAF leaf;
    
    LEAF_init(&leaf, SAMPLERATE, mempool, AUDIO_BUFFER_SIZE, &exampleRandom);
    
    tSawtooth_init(&sawtooth, &leaf);
    tSawtooth_setFreq(&sawtooth, 440);
    
    tHermiteDelay_init(&delay, 5000, 15000, &leaf);
    tHermiteDelay_setDelay(&delay, 8000);
    tHermiteDelay_setGain(&delay, 0.2f);

    for (int i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
    	audioBuffer[i] = sequencer(i);
        audioBuffer[i] += tHermiteDelay_tick(&delay, audioBuffer[i]);
    }

    saveSample(audioBuffer, AUDIO_BUFFER_SIZE, SAMPLERATE);
    return 0;
}

