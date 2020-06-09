// engine.c

#include <portaudio.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "instrument.h"
#include "engine.h"

typedef struct Engine {
  int32_t sample_rate;
  int32_t frames_per_buffer;
  PaStream* stream;
  PaStreamParameters in_port, out_port;
} Engine;

int32_t engine_time = 0;

static Engine engine;

static int32_t stereo_callback(const void* in_buff, void* out_buff, uint64_t frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data);
static int32_t open_stream();

int32_t stereo_callback(const void* in_buff, void* out_buff, uint64_t frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data) {
  float* out = (float*)out_buff;
  (void)in_buff;
  (void)time_info;
  (void)flags;
  (void)user_data;

  for (int32_t i = 0; i < (int32_t)frames_per_buffer; i++) {
    float frame = instrument_process();
    *out++ = frame;
    *out++ = frame;
    engine_time++;
  }
  int32_t note_freq = rand() % 36;
  int32_t r = rand() % 4;
  int32_t i = rand() % 4;
  if (!r) {
    instrument_change_note_freq(i, NOTE_FREQ(note_freq));
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

int32_t engine_init(int32_t sample_rate, int32_t frames_per_buffer) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[portaudio error]: %s\n", Pa_GetErrorText(err));
    return -1;
  }
  engine.sample_rate = sample_rate;
  engine.frames_per_buffer = frames_per_buffer;

  engine.out_port.device = Pa_GetDefaultOutputDevice();
  engine.out_port.channelCount = 2;
  engine.out_port.sampleFormat = paFloat32;
  engine.out_port.suggestedLatency = Pa_GetDeviceInfo(engine.out_port.device)->defaultLowOutputLatency;
  engine.out_port.hostApiSpecificStreamInfo = NULL;

  instrument_init();
  return 0;
}

int32_t engine_start() {
  open_stream();
  Pa_StartStream(engine.stream);
  getchar();
  return 0;
}

void engine_free() {
  Pa_Terminate();
}
