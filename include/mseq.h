// mseq.h

#ifndef _MSEQ_H
#define _MSEQ_H

#include <portaudio.h>

#include "common.h"
#include "instrument.h"
#include "effect.h"
#include "waveforms.h"

#define INSTRUMENTS_MAX 4
#define EFFECTS_MAX 8

typedef struct Engine {
  int32_t sample_rate;
  int32_t frames_per_buffer;
  PaStream* stream;
  PaStreamParameters in_port, out_port;
  struct Instrument instruments[INSTRUMENTS_MAX];
  int32_t instrument_count;
  Effect effect_stack[EFFECTS_MAX];
  int32_t effect_count;
  int32_t tick;
  float delta_time;
  float time;
  float tempo;
  callback_func sequence_begin;
  uint8_t is_playing;
} Engine;

extern Engine engine;

int32_t mseq_init(int32_t output_device_id, callback_func sequence_begin, int32_t sample_rate, int32_t frames_per_buffer);

void mseq_toggle_play();

void mseq_set_tempo(int32_t tempo);

struct Instrument* mseq_add_instrument();

void mseq_add_effect(Effect effect);

void mseq_clear_effects();

int32_t mseq_start(callback_func callback);

void mseq_free();

#endif
