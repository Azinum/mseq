// instrument.c

#include <assert.h>
#include <stdio.h>

#include "common.h"
#include "engine.h"
#include "mseq.h"
#include "waveforms.h"
#include "instrument.h"

enum Note_state {
  STATE_NONE,
  STATE_ATTACK,
  STATE_HOLD,
  STATE_RELEASE,
};

typedef float (*proc_func)(float, float);

struct Note_info {
  float freq;
  int32_t note_value;
  float amp;
  float release_speed;
  float attack_speed;
  int hold_time;
  int time;
  int state;
  proc_func process;
};

static int32_t index = 0;
static int32_t step = 0;
static int32_t tempo = (60 * 60) / 250;
static struct Note_info seq_table[] = {
  {0, 24, 0, 0.00001f, 0.01f, 5000, 0, STATE_NONE, wf_sine},
  {0, 20, 0, 0.00001f, 0.01f, 5000, 0, STATE_NONE, wf_sine},
  {0, 21, 0, 0.00001f, 0.01f, 5000, 0, STATE_NONE, wf_sine},
  {0, 27, 0, 0.00001f, 0.01f, 5000, 0, STATE_NONE, wf_sine},
};

#define BAR_LENGTH 16

static int32_t bar_seq[BAR_LENGTH] = {0};

static float amp_max = 0.2f;

void instrument_init() {
  for (int32_t i = 0; i < BAR_LENGTH; i++)
    bar_seq[i] = -1;
  bar_seq[0] = 0;

  bar_seq[1] = 1;
  bar_seq[2] = 2;
  bar_seq[3] = 3;

  bar_seq[4] = 1;
  bar_seq[8] = 2;
  bar_seq[12] = 3;
}

float instrument_process() {
  float result = 0;
  struct Note_info* current_note = &seq_table[index];
  /*if (!(engine_time % (tempo * FRAMES_PER_BUFFER))) {
    index = (index + 1) % (ARR_SIZE(seq_table));
    current_note->freq = NOTE_FREQ(current_note->note_value);
    current_note->state = STATE_ATTACK;
    current_note->amp = 0;
  }*/
  if (!(engine_time % (tempo * FRAMES_PER_BUFFER))) {
    printf("%i", step);
    if (bar_seq[step] >= 0) {
      printf(" *");
      index = bar_seq[step];
      assert(index < (int32_t)ARR_SIZE(seq_table));
      current_note->freq = NOTE_FREQ(current_note->note_value);
      current_note->state = STATE_ATTACK;
      current_note->amp = 0;
    }
    printf("\n");
    step = (step + 1) % BAR_LENGTH;
  }
  for (int32_t i = 0; i < (int32_t)ARR_SIZE(seq_table); i++) {
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
    result += note->process(note->amp, note->freq);
  }
  return result;
}

void instrument_change_note_freq(int32_t index, int32_t note_value) {
  assert(index < (int32_t)ARR_SIZE(seq_table));
  struct Note_info* note = &seq_table[index];
  note->note_value = note_value;
}
