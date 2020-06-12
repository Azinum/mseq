// engine.h

#ifndef _ENGINE_H
#define _ENGINE_H

extern int32_t engine_time;

int32_t engine_init(int32_t output_device_id, int32_t sample_rate, int32_t frames_per_buffer);

int32_t engine_start(callback_func callback);

void engine_free();

#endif
