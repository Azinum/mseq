// effect.c

#include "common.h"
#include "effect.h"

inline float effect_bitcrush(float input, float mix, float amount) {
  float result = 0;
  float dry = 1 - mix;
  float wet = 1 - dry;
  int8_t crush = (int8_t)(amount * input);
  result = crush / amount;
  return (input * dry) + (result * wet);
}

inline float effect_distortion(float input, float mix, float amount) {
  float result = input;
  float dry = 1 - mix;
  float wet = 1 - dry;
  result *= amount;
  if (result >= 1.0f)
    result = 1.0f / amount;
  return (input * dry) + (result * wet);
}
