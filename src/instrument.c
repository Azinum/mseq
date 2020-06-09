// instrument.c

#include "common.h"
#include "engine.h"
#include "mseq.h"
#include "waveforms.h"
#include "instrument.h"

#define STANDARD_PITCH 220
#define NOTE_FREQ(SEMI_TONE) (STANDARD_PITCH * powf(2, SEMI_TONE / 12.0f))
#define N(FREQ) NOTE_FREQ(FREQ)

enum Note_state {
  STATE_NONE,
  STATE_ATTACK,
  STATE_HOLD,
  STATE_RELEASE,
};

struct Note_info {
  float freq;
  float amp;
  float release_speed;
  float attack_speed;
  int hold_time;
  int time;
  int state;
};

static int index = 0;
static int tempo = 40;
static struct Note_info seq_table[] = {
  {0, 0, 0.00001f, 0.05f, 5000, 0, STATE_NONE},
  {0, 0, 0.00001f, 0.05f, 5000, 0, STATE_NONE},
  {0, 0, 0.00001f, 0.05f, 5000, 0, STATE_NONE},
  {0, 0, 0.00001f, 0.05f, 5000, 0, STATE_NONE},
};

static float amp_max = 0.2f;

void instrument_init() {
  seq_table[0].freq = N(0);
  seq_table[1].freq = N(3);
  seq_table[2].freq = N(0);
  seq_table[3].freq = N(7);
}

float instrument_process() {
  float result = 0;
  struct Note_info* current_note = &seq_table[index];
  if (!(engine_time % (tempo * FRAMES_PER_BUFFER))) {
    index = (index + 1) % (ARR_SIZE(seq_table));
    current_note->state = STATE_ATTACK;
    current_note->amp = 0;
  }
  for (int i = 0; i < (int)ARR_SIZE(seq_table); i++) {
    struct Note_info* note = &seq_table[i];
    switch (note->state) {
      case STATE_ATTACK: {
        note->amp += note->attack_speed;
        if (note->amp > amp_max) {
          note->amp = amp_max;
          note->state = STATE_HOLD;
          note->time = engine_time;
        }
        break;
      }
      case STATE_HOLD: {
        if (note->time + note->hold_time < engine_time)
          note->state = STATE_RELEASE;
        break;
      }
      case STATE_RELEASE: {
        note->amp -= note->release_speed;
        if (note->amp <= 0)
          note->amp = 0;
        break;
      }
      default:
        break;
    }
    result += wf_square(note->amp, note->freq);
  }
  return result;
}
