// mseq.h

#ifndef _MSEQ_H
#define _MSEQ_H

#include "common.h"

extern int32_t engine_time;

int32_t mseq_init(int32_t output_device_id, int32_t sample_rate, int32_t frames_per_buffer);

int32_t mseq_get_sample_rate();

int32_t mseq_start(callback_func callback);

void mseq_free();

#endif
