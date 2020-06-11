// instrument.h

#ifndef _INSTRUMENT_H
#define _INSTRUMENT_H

#define STANDARD_PITCH 220 / 2
#define NOTE_FREQ(SEMI_TONE) (STANDARD_PITCH * powf(2, SEMI_TONE / 12.0f))
#define N(FREQ) NOTE_FREQ(FREQ)

#define BAR_LENGTH 16
#define MAX_SEQ_NODES 8

enum Note_property {
  NP_VALUE = 1,
  NP_RELEASE_SPEED,
  NP_ATTACK_SPEED,
  NP_HOLD_TIME,
  NP_PROCESS
};

typedef union Note_property_value {
  float f;
  int32_t i;
  void* p;
} Note_property_value;

typedef float (*proc_func)(float, float);

struct Note_info {
  float freq;
  int32_t note_value;
  float amp;
  float release_speed;
  float attack_speed;
  int32_t hold_time;
  int32_t time;
  int32_t state;
  proc_func process;
};

struct Instrument {
  struct Note_info seq_table[MAX_SEQ_NODES];
  int16_t bar_seq[BAR_LENGTH];
  int16_t seq_node_count;
  int16_t index;  // NOTE(lucas): index and step are unused - they are shared between instruments
  int16_t step;
};

void instrument_init(struct Instrument* ins);

float instrument_process(struct Instrument* ins);

void instrument_change_note_freq(struct Instrument* ins, int32_t index, int32_t note_value);

int32_t instrument_add_node(struct Instrument* ins, int32_t note_value, float release_speed, float attack_speed, float hold_time, proc_func process_func);

void instrument_modify_node(struct Instrument* ins, int16_t node_id, enum Note_property prop, Note_property_value value);

#endif
