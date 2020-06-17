// effect.h

#ifndef _EFFECT_H
#define _EFFECT_H

typedef enum Effect {
  EFFECT_NONE = 0,
  EFFECT_BITCRUSH,
  EFFECT_WEIRD,
  EFFECT_DISTORTION,
  EFFECT_NOISEGATE,
  EFFECT_COMB_FILTER,
} Effect;

float effect_stack_process(float input, Effect* effect_stack, int32_t count);

extern float effect_comb_filter(float input, float mix, float amount);

extern float effect_bitcrush(float input, float mix, float amount);

extern float effect_weird(float input, float mix, float amount);

extern float effect_distortion(float input, float mix, float amount);

extern float effect_noisegate(float input, float mix, float threshold);

extern float effect_distortion2(float input, float mix, float threshold, float amount);

#endif
