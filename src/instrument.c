// instrument.c

#include "common.h"
#include "engine.h"
#include "mseq.h"
#include "instrument.h"

#define C1 100

static int32_t seq_table[] = {
  C1, C1 * 2, C1 * 2 * 2, C1 * 2 * 2 * 2
};

inline float sine_wave(float amp, float freq) {
  return amp * sin(2 * PI32 * engine_time / (SAMPLE_RATE / freq));  
}

float instrument_mix() {
  float result = sine_wave(0.1f, seq_table[0]);
  result += sine_wave(0.1f, seq_table[1]);
  return result;
}
