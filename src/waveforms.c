// waveforms.c

#include "common.h"
#include "engine.h"
#include "mseq.h"
#include "waveforms.h"

inline int sign(float n) {
  return n >= 0 ? 1 : -1;
}

inline float wf_sine(float amp, float freq) {
  return amp * sin(2 * PI32 * engine_time / (SAMPLE_RATE / freq));  
}

inline float wf_square(float amp, float freq) {
  return amp * sign(sin(2 * PI32 * engine_time / (SAMPLE_RATE / freq)));
}
