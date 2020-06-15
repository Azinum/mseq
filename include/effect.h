// effect.h

#ifndef _EFFECT_H
#define _EFFECT_H

extern float effect_bitcrush(float input, float mix, float amount);

extern float effect_distortion(float input, float mix, float amount);

extern float effect_distortion2(float input, float mix, float threshold, float amount);

#endif
