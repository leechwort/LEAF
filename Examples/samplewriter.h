/*
 ==============================================================================
   Utility for storage samples to wav file
   samplewriter.c
   Created: 21 Sep 2023
   Author:  Artem Synytsyn

 ==============================================================================
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>

int saveSample(float data[], size_t length, int samplerate);