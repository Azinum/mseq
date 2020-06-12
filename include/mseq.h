// mseq.h

#ifndef _MSEQ_H
#define _MSEQ_H

#include "common.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

int32_t mseq_exec(int argc, char** argv);

int32_t mseq_start(callback_func callback);

int32_t mseq_init(int32_t sample_rate, int32_t frames_per_buffer);

#endif
