// main.c
// mseq - mini sequencer
// author: lucas (azinum)

#include "mseq.h"
#include "common.h"

struct Args {
  int32_t output_device_id;
  int32_t sample_rate;
  int32_t frames_per_buffer;
};

static void some_func();
static void args_parse(struct Args* args, int argc, char** argv);

void some_func() {}

void args_parse(struct Args* args, int argc, char** argv) {
  for (int32_t i = 0; i < argc; i++) {
    char* arg = argv[i];
    if (arg[0] == '-') {
      switch (arg[1]) {
        case 'd': {
          if (i + 1 >= argc) {
            fprintf(stderr, "Missing argument\n");
            break;
          }
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

int main(int argc, char** argv) {
  mseq_init(-1, 44100, 512);
  mseq_start(some_func);
  mseq_free();
  return 0;
}
