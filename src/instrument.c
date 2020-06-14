// instrument.c

#include <assert.h>
#include <stdio.h>

#include "common.h"
#include "mseq.h"
#include "waveforms.h"
#include "instrument.h"

enum Note_state {
  STATE_NONE,
  STATE_ATTACK,
  STATE_HOLD,
  STATE_RELEASE,
};

static int32_t tempo = (60 * 60 * 60) / 120;

static float amp_max = 0.2f;

void instrument_init(struct Instrument* ins) {
  ins->seq_node_count = 0;
  ins->index = 0;
  ins->step = 0;
  ins->state = I_ACTIVE;
  for (int32_t i = 0; i < BAR_LENGTH; i++)
    ins->bar_seq[i] = -1;
}

void instrument_play_note(struct Instrument* ins, int16_t id) {
  assert(id >= 0 && id < MAX_SEQ_NODES);
  struct Note_info* note = &ins->seq_table[id];
  note->freq = NOTE_FREQ(note->note_value);
  note->state = STATE_ATTACK;
  note->amp = 0;
}

float instrument_process(struct Instrument* ins) {
  float result = 0;
  if (!(engine_time % tempo) && engine_is_playing) {
    if (ins->bar_seq[ins->step] >= 0) {
      ins->index = ins->bar_seq[ins->step];
      instrument_play_note(ins, ins->index);
    }
    ins->step = (ins->step + 1) % BAR_LENGTH;
  }
  for (int32_t i = 0; i < MAX_SEQ_NODES; i++) {
    struct Note_info* note = &ins->seq_table[i];
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
    if (note->process != NULL)
      result += note->process(note->amp, note->freq);
  }
  return result;
}

void instrument_change_note_freq(struct Instrument* ins, int32_t index, int32_t note_value) {
  struct Note_info* note = &ins->seq_table[index];
  note->note_value = note_value;
}

void instrument_connect_note(struct Instrument* ins, int32_t location, int32_t id) {
  assert(ins != NULL);
  assert(id < MAX_SEQ_NODES);
  assert(location < BAR_LENGTH);
  ins->bar_seq[location] = id;
}

int32_t instrument_add_note(struct Instrument* ins, int32_t note_value, float release_speed, float attack_speed, float hold_time, proc_func process_func) {
  assert(ins != NULL);
  int32_t id = ins->seq_node_count;
  if (id >= MAX_SEQ_NODES) {
    printf("Failed to add node to instrument\n");
    return -1;
  }
  struct Note_info* note = &ins->seq_table[ins->seq_node_count++];
  note->note_value = note_value;
  note->release_speed = release_speed;
  note->attack_speed = attack_speed;
  note->hold_time = hold_time;
  note->process = process_func;
  return id;
}
