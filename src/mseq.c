// mseq.c

#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

// Linux/Unix/Mac specific time measurements
#include <sys/time.h>

#include "common.h"
#include "instrument.h"
#include "waveforms.h"
#include "effect.h"
#include "mseq.h"

Engine engine;
static struct timeval old, new;
static uint8_t first_step = 0;

static int32_t stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data);
static int32_t open_stream();

int32_t stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data) {
  float* out = (float*)out_buff;
  (void)in_buff; (void)time_info; (void)flags; (void)user_data;
  float elapsed_time;
  old = new;
  for (int32_t i = 0; i < (int32_t)frames_per_buffer; i++) {
    float frame = 0;
    for (int32_t j = 0; j < INSTRUMENTS_MAX; j++) {
      struct Instrument* ins = &engine.instruments[j];
      if (ins->state == I_ACTIVE)
        frame += instrument_process(ins);
    }
    frame = effect_stack_process(frame, engine.effect_stack, engine.effect_count);
    *out++ = frame;
    *out++ = frame;
    engine.tick++;
  }
  struct Instrument* ins = &engine.instruments[0];
  if (ins->step == 0 && !first_step) {
    if (engine.sequence_begin != 0)
      engine.sequence_begin();
    first_step = 1;
  }
  else if (ins->step != 0){
    first_step = 0;
  }
#if 0
  gettimeofday(&new, NULL);
  elapsed_time = (new.tv_sec - old.tv_sec) * 1000.0f;
  elapsed_time += (new.tv_usec - old.tv_usec) / 1000.0f;
  engine.delta_time = elapsed_time / 1000.0f;
  if (engine.delta_time >= 1.0f) {
    engine.delta_time = 1.0f;
  }
  engine.time += engine.delta_time;
#endif
  engine.delta_time = (1.0f / engine.sample_rate) * frames_per_buffer;
  engine.time += engine.delta_time;
  return paContinue;
}

int32_t open_stream() {
  PaError err = Pa_OpenStream(
    &engine.stream,
    NULL,
    &engine.out_port,
    engine.sample_rate,
    engine.frames_per_buffer,
    0,
    stereo_callback, // Callback function that does the audio processing
    NULL  // User data
  );
  if (err != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[portaudio error]: %s\n", Pa_GetErrorText(err));
    return -1;
  }
  return 0;
}

int32_t mseq_init(int32_t output_device_id, callback_func sequence_begin, int32_t sample_rate, int32_t frames_per_buffer) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[portaudio error]: %s\n", Pa_GetErrorText(err));
    return -1;
  }
  engine.sample_rate = sample_rate;
  engine.frames_per_buffer = frames_per_buffer;
  engine.instrument_count = 0;
  engine.effect_count = 0;
  engine.tick = 0;
  engine.delta_time = 0;
  engine.time = 0;
  engine.sequence_begin = sequence_begin;
  engine.is_playing = 1;
  mseq_set_tempo(250);
  int32_t device_count = Pa_GetDeviceCount();
  int32_t output_device = output_device_id % device_count;
  if (output_device_id < 0)
    output_device = Pa_GetDefaultOutputDevice();
  printf("Avaliable output devices:\n");
  for (int32_t i = 0; i < device_count; i++) {
    const PaDeviceInfo* device_info = Pa_GetDeviceInfo(i);
    printf("%-2i | %s", i, device_info->name);
    if (output_device == i)
      printf(" [selected]");
    printf("\n");
  }
  engine.out_port.device = output_device;
  engine.out_port.channelCount = 2;
  engine.out_port.sampleFormat = paFloat32;
  engine.out_port.suggestedLatency = Pa_GetDeviceInfo(engine.out_port.device)->defaultHighOutputLatency;
  engine.out_port.hostApiSpecificStreamInfo = NULL;
#if !defined(COMP_SHARED_LIB)
  struct Instrument* ins = mseq_add_instrument();
  //         note value | volume | attack | hold | release | oscillator type
  instrument_add_note(ins, 24, 0.5f, 0.5, 0.1, 50, OSC_SAW);
  instrument_add_note(ins, 12, 0.5f, 0.5, 0.1, 50, OSC_SINE);
  instrument_add_note(ins, 24, 0.5f, 0.5, 0.1, 50, OSC_SAW);
  instrument_add_note(ins, 12, 0.5f, 0.5, 0.1, 50, OSC_SINE);
  instrument_connect_note(ins, 0, 0);
  instrument_connect_note(ins, 1, 1);
  instrument_connect_note(ins, 2, 2);
  instrument_connect_note(ins, 3, 3);

  int32_t rythm[] = {0, 2, 8, 12};
  instrument_set_rythm(ins, ARR_SIZE(rythm), rythm);

 //  mseq_add_effect(EFFECT_COMB_FILTER);
#endif
  return 0;
}

void mseq_toggle_play() {
  engine.is_playing = !engine.is_playing;
}

void mseq_set_tempo(int32_t tempo) {
  engine.tempo = 60.0f / tempo;
}

struct Instrument* mseq_add_instrument() {
  assert(engine.instrument_count < INSTRUMENTS_MAX);
  struct Instrument* ins = &engine.instruments[engine.instrument_count++];
  instrument_init(ins);
  return ins;
}

void mseq_add_effect(Effect effect) {
  if (engine.effect_count >= EFFECTS_MAX) {
    fprintf(stderr, "Effect stack is full\n");
    return;
  }
  engine.effect_stack[engine.effect_count++] = effect;
}

void mseq_clear_effects() {
  engine.effect_count = 0;
}

int32_t mseq_start(callback_func callback) {
  assert(callback != NULL);
  if (open_stream() < 0)
    return -1;
  Pa_StartStream(engine.stream);
  callback();
  return 0;
}

void mseq_free() {
  Pa_Terminate();
}
