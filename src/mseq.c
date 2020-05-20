// mseq.c

#include "common.h"
#include "engine.h"
#include "mseq.h"

int32_t mseq_start() {
  engine_init(SAMPLE_RATE, FRAMES_PER_BUFFER);
  engine_start();
  engine_free();
  return 0;
}
