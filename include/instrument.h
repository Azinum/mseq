// instrument.h

#ifndef _INSTRUMENT_H
#define _INSTRUMENT_H

#define STANDARD_PITCH 220
#define NOTE_FREQ(SEMI_TONE) (STANDARD_PITCH * powf(2, SEMI_TONE / 12.0f))
#define N(FREQ) NOTE_FREQ(FREQ)

void instrument_init();

float instrument_process();

void instrument_change_note_freq(int32_t index, float freq);

#endif
