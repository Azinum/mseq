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

static int32_t stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data);
static int32_t open_stream();

int32_t stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data) {
  float* out = (float*)out_buff;
  (void)in_buff; (void)time_info; (void)flags; (void)user_data;
  double elapsed_time;
  old = new;
  for (int32_t i = 0; i < (int32_t)frames_per_buffer; i++) {
    float frame = 0;
    for (int32_t j = 0; j < MAX_INSTRUMENTS; j++) {
      struct Instrument* ins = &engine.instruments[j];
      if (ins->state == I_ACTIVE)
        frame += instrument_process(ins);
    }
    *out++ = frame;
    *out++ = frame;
    engine.tick++;
  }
  
  gettimeofday(&new, NULL);
  elapsed_time = (new.tv_sec - old.tv_sec) * 1000.0f;
  elapsed_time += (new.tv_usec - old.tv_usec) / 1000.0f;
  engine.delta_time = elapsed_time / 1000.0f;
  if (engine.delta_time >= 0.1f)
    engine.delta_time = 0.1f;
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

int32_t mseq_init(int32_t output_device_id, int32_t sample_rate, int32_t frames_per_buffer) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[portaudio error]: %s\n", Pa_GetErrorText(err));
    return -1;
  }
  engine.sample_rate = sample_rate;
  engine.frames_per_buffer = frames_per_buffer;
  engine.tick = 0;
  engine.delta_time = 0;
  engine.time = 0;
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
#if !defined(COMP_SHARED_LIB)
  struct Instrument* ins = mseq_add_instrument();
  instrument_add_note(ins, 12, 0.00005f, 0.01f, 100, OSC_SINE);
  instrument_add_note(ins, 24, 0.00005f, 0.01f, 100, OSC_SINE);
  instrument_add_note(ins, 24, 0.00005f, 0.01f, 100, OSC_SINE);
  instrument_add_note(ins, 24, 0.00005f, 0.01f, 100, OSC_SINE);
  instrument_connect_note(ins, 0, 0);
  instrument_connect_note(ins, 4, 1);
  instrument_connect_note(ins, 8, 2);
  instrument_connect_note(ins, 12, 3);
#endif
  return 0;
}

void mseq_toggle_play() {
  engine.is_playing = !engine.is_playing;
}

struct Instrument* mseq_add_instrument() {
  assert(engine.instrument_count < MAX_INSTRUMENTS);
  struct Instrument* ins = &engine.instruments[engine.instrument_count++];
  instrument_init(ins);
  return ins;
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
