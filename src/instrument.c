// instrument.c

#include "common.h"
#include "engine.h"
#include "mseq.h"
#include "waveforms.h"
#include "instrument.h"

#define STANDARD_PITCH 220
#define NOTE_FREQ(SEMI_TONE) (STANDARD_PITCH * powf(2, SEMI_TONE / 12.0f))
#define N(FREQ) NOTE_FREQ(FREQ)

static int index = 0;
static int tempo = 20;
static int32_t seq_table[4] = {
  0, 0, 0, 0
};

void instrument_init() {
  seq_table[0] = N(0);
  seq_table[1] = N(3);
  seq_table[2] = N(5);
  seq_table[3] = N(8);
}

float instrument_process() {
  if (!(engine_time % (tempo * FRAMES_PER_BUFFER))) {
    index = (index + 1) % (ARR_SIZE(seq_table));
  }
  float result = wf_sine(0.1f, seq_table[index]);
  return result;
}
