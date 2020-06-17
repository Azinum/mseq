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

static void wait_callback();
static void args_parse(struct Args* args, int argc, char** argv);

void wait_callback() {
  printf("Press ENTER to exit program.\n");
  getchar();
}

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
  struct Args args = {
    .output_device_id = -1,
    .sample_rate = 44100,
    .frames_per_buffer = 512
  };
  args_parse(&args, argc, argv);
  mseq_init(args.output_device_id, NULL, args.sample_rate, args.frames_per_buffer);
  mseq_start(wait_callback);
  mseq_free();
  return 0;
}
