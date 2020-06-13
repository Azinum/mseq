// mseq.h

#ifndef _MSEQ_H
#define _MSEQ_H

#include "common.h"
#include "instrument.h"
#include "waveforms.h"

extern int32_t engine_time;
extern int32_t engine_tick;

int32_t mseq_init(int32_t output_device_id, int32_t sample_rate, int32_t frames_per_buffer);

struct Instrument* mseq_add_instrument();

int32_t mseq_get_sample_rate();

int32_t mseq_get_frames_per_buffer();

int32_t mseq_start(callback_func callback);

void mseq_free();

#endif
