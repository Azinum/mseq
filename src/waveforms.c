// waveforms.c

#include "common.h"
#include "engine.h"
#include "mseq.h"
#include "waveforms.h"

inline float wf_sine(float amp, float freq) {
  return amp * sin(2 * PI32 * engine_time / (SAMPLE_RATE / freq));  
}
