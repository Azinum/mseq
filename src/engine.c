// engine.c

#include <portaudio.h>
#include <stdio.h>
#include <math.h>

#include "common.h"
#include "engine.h"

#define PI32 3.14159265359f
#define ARR_SIZE(ARR) ((sizeof(ARR)) / (sizeof(ARR[0])))

typedef struct Engine {
  int32_t sample_rate;
  int32_t frames_per_buffer;
  PaStream* stream;
  PaStreamParameters in_port, out_port;
} Engine;

static Engine engine;
static int32_t time = 0;
static int32_t index = 0;

static int32_t stereo_callback(const void* in_buff, void* out_buff, uint64_t frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data);
static float sine_wave(float amp, float freq);
static int32_t open_stream();

#define C1 100

static int32_t seq_table[] = {
  C1, C1 * 2, C1 * 2 * 2, C1 * 2 * 2 * 2
};

int32_t stereo_callback(const void* in_buff, void* out_buff, uint64_t frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data) {
  float* out = (float*)out_buff;
  const int32_t freq = 200;
  int32_t delay = 16 * frames_per_buffer;

  for (int32_t i = 0; i < (int32_t)frames_per_buffer; i++) {
    if (!(time % delay)) {
      index = (index + 1) % ARR_SIZE(seq_table);
    }
    *out++ = sine_wave(0.1f, seq_table[index]);
    *out++ = sine_wave(0.1f, seq_table[index]);
    time++;
  }
  return paContinue;
}

float sine_wave(float amp, float freq) {
  return amp * sin(2 * PI32 * time / (engine.sample_rate / freq));  
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
  return 0;
}

int32_t engine_start() {
  open_stream();
  Pa_StartStream(engine.stream);
  getchar();
  return 0;
}

void engine_free() {

}
