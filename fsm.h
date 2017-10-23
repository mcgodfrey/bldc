#ifndef FSM_H_
#define FSM_H_

#include "timer16.h"
#include "Arduino.h"

typedef enum {
  NONE = -1,
  IDLE,
  START,
  CHECK_FEEDBACK,
  UPDATE_COMMUTATION,
  ADC_NOISE_DELAY,
  SETUP_VIN,
  MEAS_VIN,
  SETUP_VREF,
  MEAS_VREF,
  SETUP_BEMF,
  MEAS_BEMF
} state_t;


typedef void (*state_fn)(void);

typedef enum {
  AUTO,
  ADC_READY,
  TIMER1A_INT,
  TIMER1B_INT
} event_t;


typedef struct {
  state_t current_state;
  event_t event;
  state_t new_state;
} transition_t;

/* Example transition table
state_t transition_table2[][] = {
  {START,              AUTO,        IDLE},
  {IDLE,               AUTO,        CHECK_FEEDBACK},
  {CHECK_FEEDBACK,     AUTO,        UPDATE_COMMUTATION},
  {UPDATE_COMMUTATION, AUTO,        SETUP_BEMF},
  {SETUP_BEMF,         ADC_READY,   MEAS_BEMF},
  {MEAS_BEMF,          ADC_READY,   MEAS_BEMF},
  {MEAS_BEMF,          TIMER1A_INT, CHECK_FEEDBACK}
};
*/



event_t get_event(timer16_t *timer1a, timer16_t *timer1b);
state_t get_transition(transition_t transition_table[], byte num_rows, state_t state, event_t event);

#endif //FSM_H_

/*
http://www.pezzino.ch/state-machine-in-c/
https://www.embedded.com/design/prototyping-and-development/4442212/1/Implementing-finite-state-machines-in-embedded-systems
https://aticleworld.com/state-machine-using-c/
*/
