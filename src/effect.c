// effect.c

#include "common.h"
#include "mseq.h"
#include "effect.h"

#define MIX_FACTOR 0.5f
#define AMOUNT 10

float effect_stack_process(float input, Effect* effect_stack, int32_t count) {
  float result = input;
  for (int32_t i = 0; i < count; i++) {
    switch (effect_stack[i]) {
      case EFFECT_BITCRUSH:
        result += effect_bitcrush(result, MIX_FACTOR, AMOUNT);
        break;
      case EFFECT_WEIRD:
        result += effect_weird(result, MIX_FACTOR, AMOUNT);
        break;
      case EFFECT_DISTORTION:
        result += effect_distortion(result, MIX_FACTOR, AMOUNT);
        break;
      case EFFECT_NOISEGATE:
        result += effect_noisegate(result, 1.0f, 0.1f);
        break;
      default:
        break;
    }
  }
  return result;
}

inline float effect_bitcrush(float input, float mix, float amount) {
  float result = 0;
  float dry = 1 - mix;
  float wet = 1 - dry;
  int8_t crush = (int8_t)(amount * input);
  result = crush / amount;
  return (input * dry) + (result * wet);
}

inline float effect_weird(float input, float mix, float amount) {
  float result = input;
  float dry = 1 - mix;
  float wet = 1 - dry;
  result += sin(engine.tick * input * amount);
  result /= amount;
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

inline float effect_noisegate(float input, float mix, float threshold) {
  float result = input;
  float dry = 1 - mix;
  float wet = 1 - dry;
  if (fabs(input) <= threshold) {
    result = 0;
  }
  return (input * dry) + (result * wet);
}

inline float effect_distortion2(float input, float mix, float threshold, float amount) {
  float result = input;
  float dry = 1 - mix;
  float wet = 1 - dry;
  if (input >= 0.0f) {
    result = fmin(input, threshold);
  }
  else {
    result = fmax(input, -threshold);
  }
  result /= threshold / amount;
  return (input * dry) + (result * wet);
}
