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

static float amp_max = 0.2f;

void instrument_init(struct Instrument* ins) {
  ins->seq_node_count = 0;
  ins->index = 0;
  ins->state = I_ACTIVE;
  ins->time = engine.time;
  ins->step = 0;
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
  if (engine.time >= ins->time + (engine.tempo / 4.0f) && engine.is_playing) {
    if (ins->bar_seq[ins->step] >= 0) {
      ins->index = ins->bar_seq[ins->step];
      instrument_play_note(ins, ins->index);
    }
    ins->step = (ins->step + 1) % BAR_LENGTH;
    ins->time = engine.time;
  }

  for (int32_t i = 0; i < MAX_SEQ_NODES; i++) {
    struct Note_info* note = &ins->seq_table[i];
    switch (note->state) {
      case STATE_ATTACK: {
        note->amp += note->attack_speed;
        if (note->amp > amp_max) {
          note->amp = amp_max;
          note->state = STATE_HOLD;
          note->time = engine.tick;
        }
        break;
      }
      case STATE_HOLD: {
        if (note->time + note->hold_time < engine.tick)
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
    switch (note->osc_type) {
      case OSC_SINE:
        result += wf_sine(note->amp, note->freq);
        break;
      case OSC_TRIANGLE:
        result += wf_triangle(note->amp, note->freq);
        break;
      case OSC_SQUARE:
        result += wf_square(note->amp, note->freq);
        break;
      case OSC_SAW:
        result += wf_saw(note->amp, note->freq);
        break;
    }
  }
  return result;
}

// TODO(lucas): Validate that id is within range for the property modify/change functions
void instrument_change_note_freq(struct Instrument* ins, int32_t id, int32_t note_value) {
  struct Note_info* note = &ins->seq_table[id];
  note->note_value = note_value;
}

void instrument_change_attack(struct Instrument* ins, int32_t id, float value) {
  struct Note_info* note = &ins->seq_table[id];
  note->attack_speed = value;
}

void instrument_change_hold(struct Instrument* ins, int32_t id, float value) {
  struct Note_info* note = &ins->seq_table[id];
  note->hold_time = value;
}

void instrument_change_release(struct Instrument* ins, int32_t id, float value) {
  struct Note_info* note = &ins->seq_table[id];
  note->release_speed = value;
}

void instrument_change_osc(struct Instrument* ins, int32_t id, Oscillator osc_type) {
  struct Note_info* note = &ins->seq_table[id];
  note->osc_type = osc_type;
}

void instrument_connect_note(struct Instrument* ins, int32_t location, int32_t id) {
  assert(ins != NULL);
  assert(id < MAX_SEQ_NODES);
  assert(location < BAR_LENGTH);
  ins->bar_seq[location] = id;
}

int32_t instrument_add_note(struct Instrument* ins, int32_t note_value, float release_speed, float attack_speed, float hold_time, Oscillator osc_type) {
  assert(ins != NULL);
  int32_t id = ins->seq_node_count;
  if (id >= MAX_SEQ_NODES) {
    printf("Failed to add node to instrument\n");
    return -1;
  }
  struct Note_info* note = &ins->seq_table[ins->seq_node_count++];
  note->note_value = note_value;
  note->freq = NOTE_FREQ(note_value);
  note->release_speed = release_speed;
  note->attack_speed = attack_speed;
  note->hold_time = hold_time;
  note->osc_type = osc_type;
  return id;
}
