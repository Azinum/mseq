// mseq.c

#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "engine.h"
#include "mseq.h"

struct Args {
  int32_t output_device_id;
};

static void args_parse(struct Args* args, int argc, char** argv);

void args_parse(struct Args* args, int argc, char** argv) {
  for (int32_t i = 0; i < argc; i++) {
    char* arg = argv[i];
    if (arg[0] == '-') {
      switch (arg[1]) {
        case 'd': {
          char* value = argv[i + 1];
          int32_t id = atoi(value);
          args->output_device_id = id;
          i++;
          break;
        }
        default:
          break;
      }
    }
  }
}

int32_t mseq_start(int argc, char** argv) {
  struct Args args = {
    .output_device_id = -1,
  };
  args_parse(&args, argc, argv);
  engine_init(args.output_device_id, SAMPLE_RATE, FRAMES_PER_BUFFER);
  engine_start();
  engine_free();
  return 0;
}
