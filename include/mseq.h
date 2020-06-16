// mseq.h

#ifndef _MSEQ_H
#define _MSEQ_H

#include <portaudio.h>

#include "common.h"
#include "instrument.h"
#include "waveforms.h"

#define MAX_INSTRUMENTS 4

typedef struct Engine {
  int32_t sample_rate;
  int32_t frames_per_buffer;
  PaStream* stream;
  PaStreamParameters in_port, out_port;
  struct Instrument instruments[MAX_INSTRUMENTS];
  int32_t instrument_count;
  int32_t tick;
  double delta_time;
  double time;
  float tempo;
  uint8_t is_playing;
} Engine;

extern Engine engine;

int32_t mseq_init(int32_t output_device_id, int32_t sample_rate, int32_t frames_per_buffer);

void mseq_toggle_play();

void mseq_set_tempo(int32_t tempo);

struct Instrument* mseq_add_instrument();

int32_t mseq_start(callback_func callback);

void mseq_free();

#endif
