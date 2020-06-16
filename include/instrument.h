// instrument.h

#ifndef _INSTRUMENT_H
#define _INSTRUMENT_H

#define STANDARD_PITCH 220 / 2
#define NOTE_FREQ(SEMI_TONE) (STANDARD_PITCH * powf(2, SEMI_TONE / 12.0f))
#define N(FREQ) NOTE_FREQ(FREQ)

#define BAR_LENGTH 16
#define MAX_SEQ_NODES 8

typedef float (*proc_func)(float, float);

typedef enum Oscillator {
  OSC_SINE,
  OSC_TRIANGLE,
  OSC_SQUARE,
  OSC_SAW,
} Oscillator;

struct Note_info {
  float freq;
  int32_t note_value;
  float amp;
  float release_speed;
  float attack_speed;
  int32_t hold_time;
  int32_t time;
  int32_t state;
  Oscillator osc_type;
};

enum Instrument_state {
  I_INACTIVE = 0,
  I_ACTIVE
};

struct Instrument {
  struct Note_info seq_table[MAX_SEQ_NODES];
  int16_t bar_seq[BAR_LENGTH];
  int16_t seq_node_count;
  int16_t index;
  int16_t state;
  int16_t step;
  float time;
};

void instrument_init(struct Instrument* ins);

void instrument_play_note(struct Instrument* ins, int16_t id);

void instruments_update();

float instrument_process(struct Instrument* ins);

void instrument_change_note_freq(struct Instrument* ins, int32_t index, int32_t note_value);

void instrument_change_attack(struct Instrument* ins, int32_t id, float value);

void instrument_change_hold(struct Instrument* ins, int32_t id, float value);

void instrument_change_release(struct Instrument* ins, int32_t id, float value);

void instrument_change_osc(struct Instrument* ins, int32_t id, Oscillator osc_type);

void instrument_connect_note(struct Instrument* ins, int32_t location, int32_t id);

int32_t instrument_add_note(struct Instrument* ins, int32_t note_value, float release_speed, float attack_speed, float hold_time, Oscillator osc_type);

#endif
