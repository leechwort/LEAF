/*
 ==============================================================================
   Utility for storage samples to wav file.
   samplewriter.c
   Created: 21 Sep 2023
   Author:  Artem Synytsyn

 ==============================================================================
*/

#include "samplewriter.h"
#include <sndfile.h>

int saveSample(float data[], size_t length, int samplerate)
{
    const char *filename = "output.wav";

    // Open the audio file for writing
    SF_INFO sfinfo;
    sfinfo.samplerate = samplerate;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    SNDFILE *outfile = sf_open(filename, SFM_WRITE, &sfinfo);
    if (!outfile)
    {
        printf("Error opening audio file for writing\n");
        return 1;
    }

    // Write the audio data to the file
    sf_writef_float(outfile, data, length);
    sf_close(outfile);
    return 0;
}