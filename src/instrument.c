// instrument.c

#include "common.h"
#include "engine.h"
#include "mseq.h"
#include "waveforms.h"
#include "instrument.h"

#define C1 100

static int index = 0;
static int tempo = 20;
static int32_t seq_table[] = {
  C1, C1 * 2, C1 * 2 * 2, C1 * 2 * 2 * 2
};

float instrument_process() {
  if (!(engine_time % (tempo * FRAMES_PER_BUFFER))) {
    index = (index + 1) % (ARR_SIZE(seq_table));
  }
  float result = wf_sine(0.1f, seq_table[index]);
  return result;
}
