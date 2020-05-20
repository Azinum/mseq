// mseq.c

#include "common.h"
#include "engine.h"
#include "mseq.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

int32_t mseq_start() {
  engine_init(SAMPLE_RATE, FRAMES_PER_BUFFER);
  engine_start();
  engine_free();
  return 0;
}
