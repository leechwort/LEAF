/*==============================================================================

    leaf-midi.c
    Created: 30 Nov 2018 11:29:16am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-midi.h"

#else

#include "../Inc/leaf-midi.h"

#endif

//====================================================================================
/* Stack */
//====================================================================================

void tStack_init(tStack* const stack)
{
    _tStack* ns = *stack = (_tStack*) leaf_alloc(sizeof(_tStack));
    
    ns->ordered = OFALSE;
    ns->size = 0;
    ns->pos = 0;
    ns->capacity = STACK_SIZE;
    
    for (int i = 0; i < STACK_SIZE; i++) ns->data[i] = -1;
}

void tStack_free(tStack* const stack)
{
    _tStack* ns = *stack;
    
    leaf_free(ns);
}

void    tStack_initToPool           (tStack* const stack, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tStack* ns = *stack = (_tStack*) mpool_alloc(sizeof(_tStack), m);
    
    ns->ordered = OFALSE;
    ns->size = 0;
    ns->pos = 0;
    ns->capacity = STACK_SIZE;
    
    for (int i = 0; i < STACK_SIZE; i++) ns->data[i] = -1;
}

void    tStack_freeFromPool         (tStack* const stack, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tStack* ns = *stack;
    
    mpool_free(ns, m);
}

// If stack contains note, returns index. Else returns -1;
int tStack_contains(tStack* const stack, uint16_t noteVal)
{
    _tStack* ns = *stack;
    for (int i = 0; i < ns->size; i++)
    {
        if (ns->data[i] == noteVal)    return i;
    }
    return -1;
}

void tStack_add(tStack* const stack, uint16_t noteVal)
{
    _tStack* ns = *stack;
    
    uint8_t j;
    
    int whereToInsert = 0;
    if (ns->ordered)
    {
        for (j = 0; j < ns->size; j++)
        {
            if (noteVal > ns->data[j])
            {
                if ((noteVal < ns->data[j+1]) || (ns->data[j+1] == -1))
                {
                    whereToInsert = j+1;
                    break;
                }
            }
        }
    }
    
    //first move notes that are already in the stack one position to the right
    for (j = ns->size; j > whereToInsert; j--)
    {
        ns->data[j] = ns->data[(j - 1)];
    }
    
    //then, insert the new note into the front of the stack
    ns->data[whereToInsert] = noteVal;
    
    ns->size++;
}

int tStack_addIfNotAlreadyThere(tStack* const stack, uint16_t noteVal)
{
    _tStack* ns = *stack;
    
    uint8_t j;
    
    int added = 0;
    
    if (tStack_contains(stack, noteVal) == -1)
    {
        int whereToInsert = 0;
        if (ns->ordered)
        {
            for (j = 0; j < ns->size; j++)
            {
                if (noteVal > ns->data[j])
                {
                    if ((noteVal < ns->data[j+1]) || (ns->data[j+1] == -1))
                    {
                        whereToInsert = j+1;
                        break;
                    }
                }
            }
        }
        
        //first move notes that are already in the stack one position to the right
        for (j = ns->size; j > whereToInsert; j--)
        {
            ns->data[j] = ns->data[(j - 1)];
        }
        
        //then, insert the new note into the front of the stack
        ns->data[whereToInsert] = noteVal;
        
        ns->size++;
        
        added = 1;
    }
    
    return added;
}

// Remove noteVal. return 1 if removed, 0 if not
int tStack_remove(tStack* const stack, uint16_t noteVal)
{
    _tStack* ns = *stack;
    
    uint8_t k;
    int foundIndex = tStack_contains(stack, noteVal);
    int removed = 0;
    
    if (foundIndex >= 0)
    {
        for (k = 0; k < (ns->size - foundIndex); k++)
        {
            if ((k+foundIndex) >= (ns->capacity - 1))
            {
                ns->data[k + foundIndex] = -1;
            }
            else
            {
                ns->data[k + foundIndex] = ns->data[k + foundIndex + 1];
                if ((k + foundIndex) == (ns->size - 1))
                {
                    ns->data[k + foundIndex + 1] = -1;
                }
            }
            
        }
        // in case it got put on the stack multiple times
        foundIndex--;
        ns->size--;
        removed = 1;
    }
    
    return removed;
}

// Doesn't change size of data types
void tStack_setCapacity(tStack* const stack, uint16_t cap)
{
    _tStack* ns = *stack;
    
    if (cap <= 0)
        ns->capacity = 1;
    else if (cap <= STACK_SIZE)
        ns->capacity = cap;
    else
        ns->capacity = STACK_SIZE;
    
    for (int i = cap; i < STACK_SIZE; i++)
    {
        if ((int)ns->data[i] != -1)
        {
            ns->data[i] = -1;
            ns->size -= 1;
        }
    }
    
    if (ns->pos >= cap)
    {
        ns->pos = 0;
    }
}

int tStack_getSize(tStack* const stack)
{
    _tStack* ns = *stack;
    
    return ns->size;
}

void tStack_clear(tStack* const stack)
{
    _tStack* ns = *stack;
    
    for (int i = 0; i < STACK_SIZE; i++)
    {
        ns->data[i] = -1;
    }
    ns->pos = 0;
    ns->size = 0;
}

// Next item in order of addition to stack. Return 0-31 if there is a next item to move to. Returns -1 otherwise.
int tStack_next(tStack* const stack)
{
    _tStack* ns = *stack;
    
    int step = 0;
    if (ns->size != 0) // if there is at least one note in the stack
    {
        if (ns->pos > 0) // if you're not at the most recent note (first one), then go backward in the array (moving from earliest to latest)
        {
            ns->pos--;
        }
        else
        {
            ns->pos = (ns->size - 1); // if you are the most recent note, go back to the earliest note in the array
        }
        
        step = ns->data[ns->pos];
        return step;
    }
    else
    {
        return -1;
    }
}

int tStack_get(tStack* const stack, int which)
{
    _tStack* ns = *stack;
    return ns->data[which];
}

int tStack_first(tStack* const stack)
{
    _tStack* ns = *stack;
    return ns->data[0];
}


// POLY
void tPoly_init(tPoly* const polyh, int maxNumVoices)
{
    _tPoly* poly = *polyh = (_tPoly*) leaf_alloc(sizeof(_tPoly));
    
    poly->numVoices = maxNumVoices;
    poly->maxNumVoices = maxNumVoices;
    poly->lastVoiceToChange = 0;
    
    // Arp mode stuff
    poly->currentVoice = 0;
    poly->maxLength = 128;
    poly->currentNote = -1;
    
    //default learned CCs and notes are just the CCs 1-128 - notes are skipped
    for (int i = 0; i < 128; i++)
    {
        poly->notes[i][0] = 0;
        poly->notes[i][1] = -1;
    }
    
    poly->glideTime = 5.0f;
    
    poly->ramps = (tRamp*) leaf_alloc(sizeof(tRamp) * poly->maxNumVoices);
    poly->rampVals = (float*) leaf_alloc(sizeof(float) * poly->maxNumVoices);
    poly->firstReceived = (oBool*) leaf_alloc(sizeof(oBool) * poly->maxNumVoices);
    poly->voices = (int**) leaf_alloc(sizeof(int*) * poly->maxNumVoices);
    
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        poly->voices[i] = (int*) leaf_alloc(sizeof(int) * 2);
        poly->voices[i][0] = -1;
        poly->firstReceived[i] = OFALSE;
        
        tRamp_init(&poly->ramps[i], poly->glideTime, 1);
    }
    
    poly->pitchBend = 0.0f;
    
    tRamp_init(&poly->pitchBendRamp, 1.0f, 1);
    tStack_init(&poly->stack);
    tStack_init(&poly->orderStack);
    
    poly->pitchGlideIsActive = OFALSE;
}

void tPoly_free(tPoly* const polyh)
{
    _tPoly* poly = *polyh;
    
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        tRamp_free(&poly->ramps[i]);
        leaf_free(poly->voices[i]);
    }
    tRamp_free(&poly->pitchBendRamp);
    tStack_free(&poly->stack);
    tStack_free(&poly->orderStack);
    
    leaf_free(poly->voices);
    leaf_free(poly->ramps);
    leaf_free(poly->rampVals);
    leaf_free(poly->firstReceived);
    
    leaf_free(poly);
}

void    tPoly_initToPool            (tPoly* const polyh, int maxNumVoices, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPoly* poly = *polyh = (_tPoly*) mpool_alloc(sizeof(_tPoly), m);
    
    poly->numVoices = maxNumVoices;
    poly->maxNumVoices = maxNumVoices;
    poly->lastVoiceToChange = 0;
    
    // Arp mode stuff
    poly->currentVoice = 0;
    poly->maxLength = 128;
    poly->currentNote = -1;
    
    //default learned CCs and notes are just the CCs 1-128 - notes are skipped
    for (int i = 0; i < 128; i++)
    {
        poly->notes[i][0] = 0;
        poly->notes[i][1] = -1;
    }
    
    poly->glideTime = 5.0f;
    
    poly->ramps = (tRamp*) mpool_alloc(sizeof(tRamp) * poly->maxNumVoices, m);
    poly->rampVals = (float*) mpool_alloc(sizeof(float) * poly->maxNumVoices, m);
    poly->firstReceived = (oBool*) mpool_alloc(sizeof(oBool) * poly->maxNumVoices, m);
    poly->voices = (int**) mpool_alloc(sizeof(int*) * poly->maxNumVoices, m);
    
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        poly->voices[i] = (int*) mpool_alloc(sizeof(int) * 2, m);
        poly->voices[i][0] = -1;
        poly->firstReceived[i] = OFALSE;
        
        tRamp_initToPool(&poly->ramps[i], poly->glideTime, 1, mp);
    }
    
    poly->pitchBend = 0.0f;
    
    tRamp_initToPool(&poly->pitchBendRamp, 1.0f, 1, mp);
    tStack_initToPool(&poly->stack, mp);
    tStack_initToPool(&poly->orderStack, mp);
    
    poly->pitchGlideIsActive = OFALSE;
}

void    tPoly_freeFromPool  (tPoly* const polyh, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPoly* poly = *polyh;
    
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        tRamp_freeFromPool(&poly->ramps[i], mp);
        mpool_free(poly->voices[i], m);
    }
    tRamp_freeFromPool(&poly->pitchBendRamp, mp);
    tStack_freeFromPool(&poly->stack, mp);
    tStack_freeFromPool(&poly->orderStack, mp);
    
    mpool_free(poly->voices, m);
    mpool_free(poly->ramps, m);
    mpool_free(poly->rampVals, m);
    mpool_free(poly->firstReceived, m);
    
    mpool_free(poly, m);
}

void tPoly_tickPitch(tPoly* polyh)
{
    tPoly_tickPitchGlide(polyh);
    tPoly_tickPitchBend(polyh);
}

void tPoly_tickPitchGlide(tPoly* polyh)
{
    _tPoly* poly = *polyh;
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        tRamp_tick(&poly->ramps[i]);
    }
}

void tPoly_tickPitchBend(tPoly* polyh)
{
    _tPoly* poly = *polyh;
    tRamp_tick(&poly->pitchBendRamp);
}

void tPoly_setPitchBend(tPoly* const polyh, float pitchBend)
{
    _tPoly* poly = *polyh;
    poly->pitchBend = pitchBend;
    tRamp_setDest(&poly->pitchBendRamp, poly->pitchBend);
}

int tPoly_noteOn(tPoly* const polyh, int note, uint8_t vel)
{
    _tPoly* poly = *polyh;
    
    // if not in keymap or already on stack, dont do anything. else, add that note.
    if (tStack_contains(&poly->stack, note) >= 0) return -1;
    else
    {
        tPoly_orderedAddToStack(polyh, note);
        tStack_add(&poly->stack, note);
        
        int alteredVoice = -1;
        oBool found = OFALSE;
        for (int i = 0; i < poly->numVoices; i++)
        {
            if (poly->voices[i][0] < 0)    // if inactive voice, give this note to voice
            {
                if (!poly->firstReceived[i] || !poly->pitchGlideIsActive)
                {
                    tRamp_setVal(&poly->ramps[i], note);
                    poly->firstReceived[i] = OTRUE;
                }
                
                found = OTRUE;
                
                poly->voices[i][0] = note;
                poly->voices[i][1] = vel;
                poly->lastVoiceToChange = i;
                poly->notes[note][0] = vel;
                poly->notes[note][1] = i;
                
                tRamp_setDest(&poly->ramps[i], poly->voices[i][0]);
                
                alteredVoice = i;
                break;
            }
        }
        
        if (!found) //steal
        {
            int whichVoice, whichNote;
            for (int j = tStack_getSize(&poly->stack) - 1; j >= 0; j--)
            {
                whichNote = tStack_get(&poly->stack, j);
                whichVoice = poly->notes[whichNote][1];
                if (whichVoice >= 0)
                {
                    poly->lastVoiceToChange = whichVoice;
                    int oldNote = poly->voices[whichVoice][0];
                    poly->voices[whichVoice][0] = note;
                    poly->voices[whichVoice][1] = vel;
                    poly->notes[oldNote][1] = -1; //mark the stolen voice as inactive (in the second dimension of the notes array)
                    poly->notes[note][0] = vel;
                    poly->notes[note][1] = whichVoice;
                    
                    if (poly->pitchGlideIsActive)
                    {
                    	tRamp_setTime(&poly->ramps[whichVoice], poly->glideTime);
                    }
                    else
                    {
                    	tRamp_setVal(&poly->ramps[whichVoice], note);
                    }
                    tRamp_setDest(&poly->ramps[whichVoice], poly->voices[whichVoice][0]);
                    
                    alteredVoice = whichVoice;
                    
                    break;
                }
            }
        }
        return alteredVoice;
    }
}


int16_t noteToTest = -1;

int tPoly_noteOff(tPoly* const polyh, uint8_t note)
{
    _tPoly* poly = *polyh;
    
    tStack_remove(&poly->stack, note);
    tStack_remove(&poly->orderStack, note);
    poly->notes[note][0] = 0;
    poly->notes[note][1] = -1;
    
    int deactivatedVoice = -1;
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        if (poly->voices[i][0] == note)
        {
            poly->voices[i][0] = -1;
            poly->voices[i][1] = 0;
            poly->lastVoiceToChange = i;
            deactivatedVoice = i;
            break;
        }
    }
    /*
     //monophonic handling
     if ((poly->numVoices == 1) && (tStack_getSize(poly->stack) > 0))
     {
     int oldNote = tStack_first(poly->stack);
     poly->voices[0][0] = oldNote;
     poly->voices[0][1] = poly->notes[oldNote][0];
     poly->lastVoiceToChange = 0;
     }
     */
    
    //grab old notes off the stack if there are notes waiting to replace the free voice
    if (deactivatedVoice >= 0)
    {
        for (int j = 0; j < tStack_getSize(&poly->stack); ++j)
        {
            noteToTest = tStack_get(&poly->stack, j);
            
            if (poly->notes[noteToTest][1] < 0) //if there is a stolen note waiting (marked inactive but on the stack)
            {
                poly->voices[deactivatedVoice][0] = noteToTest; //set the newly free voice to use the old stolen note
                if (poly->pitchGlideIsActive)
                {
                	tRamp_setTime(&poly->ramps[deactivatedVoice], poly->glideTime);
                }
                else
                {
                	tRamp_setVal(&poly->ramps[deactivatedVoice], noteToTest);
                }
                tRamp_setDest(&poly->ramps[deactivatedVoice], poly->voices[deactivatedVoice][0]);
                poly->voices[deactivatedVoice][1] = poly->notes[noteToTest][0]; // set the velocity of the voice to be the velocity of that note
                poly->notes[noteToTest][1] = deactivatedVoice; //mark that it is no longer stolen and is now active
                return -1;
            }
        }
    }
    return deactivatedVoice;
}

void tPoly_orderedAddToStack(tPoly* const polyh, uint8_t noteVal)
{
    _tPoly* poly = *polyh;
    
    uint8_t j;
    int myPitch, thisPitch, nextPitch;
    
    tStack ns = poly->orderStack;
    
    int whereToInsert = 0;
    
    for (j = 0; j < ns->size; j++)
    {
        myPitch = noteVal;
        thisPitch = ns->data[j];
        nextPitch = ns->data[j+1];
        
        if (myPitch > thisPitch)
        {
            if ((myPitch < nextPitch) || (nextPitch == -1))
            {
                whereToInsert = j+1;
                break;
            }
        }
    }
    
    //first move notes that are already in the stack one position to the right
    for (j = ns->size; j > whereToInsert; j--)
    {
        ns->data[j] = ns->data[(j - 1)];
    }
    
    //then, insert the new note into the front of the stack
    ns->data[whereToInsert] =  noteVal;
    
    ns->size++;
    
}

void tPoly_setNumVoices(tPoly* const polyh, uint8_t numVoices)
{
    _tPoly* poly = *polyh;
    poly->numVoices = (numVoices > poly->maxNumVoices) ? poly->maxNumVoices : numVoices;
}

void tPoly_setPitchGlideActive(tPoly* const polyh, oBool isActive)
{
    _tPoly* poly = *polyh;
    poly->pitchGlideIsActive = isActive;
}

void tPoly_setPitchGlideTime(tPoly* const polyh, float t)
{
    _tPoly* poly = *polyh;
    poly->glideTime = t;
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        tRamp_setTime(&poly->ramps[i], poly->glideTime);
    }
}

void tPoly_setBendGlideTime(tPoly* const polyh, float t)
{
    _tPoly* poly = *polyh;
    tRamp_setTime(&poly->pitchBendRamp, t);
}

void tPoly_setBendSamplesPerTick(tPoly* const polyh, float t)
{
	 _tPoly* poly = *polyh;
	 poly->pitchBendRamp->samples_per_tick =  t;
}

int tPoly_getNumVoices(tPoly* const polyh)
{
    _tPoly* poly = *polyh;
    return poly->numVoices;
}

int tPoly_getNumActiveVoices(tPoly* const polyh)
{
    _tPoly* poly = *polyh;
    return LEAF_clip(0, tStack_getSize(&poly->stack), poly->numVoices);
}

float tPoly_getPitch(tPoly* const polyh, uint8_t voice)
{
    _tPoly* poly = *polyh;
    return tRamp_sample(&poly->ramps[voice]) + tRamp_sample(&poly->pitchBendRamp);
}

int tPoly_getKey(tPoly* const polyh, uint8_t voice)
{
    _tPoly* poly = *polyh;
    return poly->voices[voice][0];
}

int tPoly_getVelocity(tPoly* const polyh, uint8_t voice)
{
    _tPoly* poly = *polyh;
    return poly->voices[voice][1];
}

int tPoly_isOn(tPoly* const polyh, uint8_t voice)
{
    _tPoly* poly = *polyh;
    return (poly->voices[voice][0] > 0) ? 1 : 0;
}



//tSimplePoly = more efficient implementation without ramps and glide


// SIMPLE POLY
void tSimplePoly_init(tSimplePoly* const polyh, int maxNumVoices)
{
    tSimplePoly_initToPool(polyh, maxNumVoices, &leaf.mempool);
}

void tSimplePoly_free(tSimplePoly* const polyh)
{
    tSimplePoly_freeFromPool(polyh, &leaf.mempool);
}

void    tSimplePoly_initToPool            (tSimplePoly* const polyh, int maxNumVoices, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimplePoly* poly = *polyh = (_tSimplePoly*) mpool_alloc(sizeof(_tSimplePoly), m);

    poly->numVoices = maxNumVoices;
    poly->maxNumVoices = maxNumVoices;

    for (int i = 0; i < 128; i++)
    {
        poly->notes[i][0] = -1;
        poly->notes[i][1] = 0;
    }

    poly->voices = (int**) mpool_alloc(sizeof(int*) * poly->maxNumVoices, m);

    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        poly->voices[i] = (int*) mpool_alloc(sizeof(int) * 3, m);
        poly->voices[i][0] = -1;
    }
    tStack_initToPool(&poly->stack, mp);

}

void    tSimplePoly_freeFromPool  (tSimplePoly* const polyh, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimplePoly* poly = *polyh;

    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        mpool_free(poly->voices[i], m);
    }
    tStack_freeFromPool(&poly->stack, mp);
    mpool_free(poly->voices, m);
    mpool_free(poly, m);
}

int tSimplePoly_noteOn(tSimplePoly* const polyh, int note, uint8_t vel)
{
    _tSimplePoly* poly = *polyh;
    int whichVoice, whichNote, oldNote, alteredVoice;
    // if not in keymap or already on stack, dont do anything. else, add that note.
    if (tStack_contains(&poly->stack, note) >= 0) return -1;
    else
    {
        tStack_add(&poly->stack, note);

        alteredVoice = -1;
        oBool found = OFALSE;
        for (int i = 0; i < poly->numVoices; i++)
        {
            if (poly->voices[i][0] < 0)    // if inactive voice, give this note to voice
            {

                found = OTRUE;

                poly->voices[i][0] = note;
                poly->voices[i][1] = vel;
                poly->notes[note][0] = i;

                poly->voices[i][2] = note; // voices[i][2] is the output midi note, (avoiding the -1 when a voice is inactive)

                alteredVoice = i;
                break;
            }
        }

        if (!found) //steal
        {
            for (int j = tStack_getSize(&poly->stack) - 1; j >= 0; j--)
            {
                whichNote = tStack_get(&poly->stack, j);
                whichVoice = poly->notes[whichNote][0];
                if (whichVoice >= 0)
                {
                    oldNote = poly->voices[whichVoice][0];
                    poly->voices[whichVoice][0] = note;
                    poly->voices[whichVoice][1] = vel;
                    poly->notes[oldNote][0] = -1; //mark the stolen voice as inactive (in the second dimension of the notes array)
                    poly->notes[note][0] = whichVoice;
                    poly->notes[note][1] = vel;

                    poly->voices[whichVoice][2] = note;

                    alteredVoice = whichVoice;

                    break;
                }
            }
        }
        return alteredVoice;
    }
}




int tSimplePoly_noteOff(tSimplePoly* const polyh, uint8_t note)
{
    _tSimplePoly* poly = *polyh;
    int16_t noteToTest = -1;


    tStack_remove(&poly->stack, note);
    poly->notes[note][0] = -1;


    int deactivatedVoice = -1;
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        if (poly->voices[i][0] == note)
        {
            poly->voices[i][0] = -1;
            poly->voices[i][1] = 0;
            deactivatedVoice = i;
            break;
        }
    }

    //grab old notes off the stack if there are notes waiting to replace the free voice
    if (deactivatedVoice >= 0)
    {
        for (int j = 0; j < tStack_getSize(&poly->stack); ++j)
        {
            noteToTest = tStack_get(&poly->stack, j);

            if (poly->notes[noteToTest][0] < 0) //if there is a stolen note waiting (marked inactive but on the stack)
            {
                poly->voices[deactivatedVoice][0] = noteToTest; //set the newly free voice to use the old stolen note
                poly->voices[deactivatedVoice][1] = poly->notes[noteToTest][1]; // set the velocity of the voice to be the velocity of that note
                poly->voices[deactivatedVoice][2] = noteToTest;
                poly->notes[noteToTest][0] = deactivatedVoice; //mark that it is no longer stolen and is now active
                return -1;
            }
        }
    }
    return deactivatedVoice;
}

void tSimplePoly_setNumVoices(tSimplePoly* const polyh, uint8_t numVoices)
{
    _tSimplePoly* poly = *polyh;
    poly->numVoices = (numVoices > poly->maxNumVoices) ? poly->maxNumVoices : numVoices;
}


int tSimplePoly_getNumVoices(tSimplePoly* const polyh)
{
    _tSimplePoly* poly = *polyh;
    return poly->numVoices;
}

int tSimplePoly_getNumActiveVoices(tSimplePoly* const polyh)
{
    _tSimplePoly* poly = *polyh;
    return LEAF_clip(0, tStack_getSize(&poly->stack), poly->numVoices);
}


int tSimplePoly_getPitch(tSimplePoly* const polyh, uint8_t voice)
{
    _tSimplePoly* poly = *polyh;
    return poly->voices[voice][2];
}

int tSimplePoly_getVelocity(tSimplePoly* const polyh, uint8_t voice)
{
    _tSimplePoly* poly = *polyh;
    return poly->voices[voice][1];
}

int tSimplePoly_isOn(tSimplePoly* const polyh, uint8_t voice)
{
    _tSimplePoly* poly = *polyh;
    return (poly->voices[voice][0] > 0) ? 1 : 0;
}
