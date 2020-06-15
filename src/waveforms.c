// waveforms.c

#include "common.h"
#include "mseq.h"
#include "waveforms.h"

inline int32_t sign(float n);
inline float saw(float n);

int32_t sign(float n) {
  return n >= 0 ? 1 : -1;
}

float saw(float n) {
  return n < 0.99f ? n : -1.0f;
}

inline float wf_sine(float amp, float freq) {
  return amp * sin(engine.tick * freq * 2 * PI32 / engine.sample_rate);
}

inline float wf_square(float amp, float freq) {
  return amp * sign(sin((engine.tick * freq * 2 * PI32) / engine.sample_rate));
}

inline float wf_saw(float amp, float freq) {
  return amp * saw(sin((fmod(engine.tick, freq) * freq * 2 * PI32) / engine.sample_rate));
}

inline float wf_triangle(float amp, float freq) {
  return amp * asin(sin((engine.tick * freq * 2 * PI32) / engine.sample_rate));
}
