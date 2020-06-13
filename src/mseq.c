// mseq.c

#include <portaudio.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "common.h"
#include "instrument.h"
#include "waveforms.h"
#include "mseq.h"

#define MAX_INSTRUMENTS 4

typedef struct Engine {
  int32_t sample_rate;
  int32_t frames_per_buffer;
  PaStream* stream;
  PaStreamParameters in_port, out_port;
  struct Instrument instruments[MAX_INSTRUMENTS];
  int32_t instrument_count;
  uint8_t is_playing;
} Engine;

int32_t engine_time = 0;
int32_t engine_tick = 0;

static Engine engine;

static int32_t stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data);
static int32_t open_stream();

// - Tempo (fixa timings och ge möjlighet att ändra tempo)
// - Tap grej
// - Conn grej
int32_t stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data) {
  float* out = (float*)out_buff;
  (void)in_buff; (void)time_info; (void)flags; (void)user_data;
  for (int32_t i = 0; i < (int32_t)frames_per_buffer; i++) {
    float frame = 0;
    if (!engine.is_playing) {
      *out++ = 0;
      *out++ = 0;
      continue;
    }
    for (int32_t j = 0; j < MAX_INSTRUMENTS; j++) {
      struct Instrument* ins = &engine.instruments[j];
      if (ins->state == I_ACTIVE)
        frame += instrument_process(ins);
    }
    *out++ = frame;
    *out++ = frame;
    engine_time++;
    engine_tick++;
  }
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

int32_t mseq_init(int32_t output_device_id, int32_t sample_rate, int32_t frames_per_buffer) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[portaudio error]: %s\n", Pa_GetErrorText(err));
    return -1;
  }
  engine.sample_rate = sample_rate;
  engine.frames_per_buffer = frames_per_buffer;
  engine.is_playing = 1;
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

  struct Instrument* ins = mseq_add_instrument();
  instrument_add_note(ins, 28, 0.000005f, 0.001f, 100, wf_sine);
  instrument_add_note(ins, 28, 0.000005f, 0.001f, 100, wf_sine);
  instrument_add_note(ins, 28, 0.000005f, 0.001f, 100, wf_sine);
  instrument_add_note(ins, 28, 0.000005f, 0.001f, 100, wf_sine);
  return 0;
}

void mseq_toggle_play() {
  engine.is_playing = !engine.is_playing;
}

struct Instrument* mseq_add_instrument() {
  assert(engine.instrument_count < MAX_INSTRUMENTS);
  struct Instrument* ins = &engine.instruments[engine.instrument_count++];
  assert(ins != NULL);
  instrument_init(ins);
  return ins;
}

inline int32_t mseq_get_sample_rate() {
  return engine.sample_rate;
}

inline int32_t mseq_get_frames_per_buffer() {
  return engine.frames_per_buffer;
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
