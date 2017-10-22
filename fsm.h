#ifndef FSM_H_
#define FSM_H_

#include "Arduino.h"

typedef enum {
  NONE = -1,
  IDLE,
  START,
  CHECK_FEEDBACK,
  UPDATE_COMMUTATION,
  SETUP_BEMF,
  MEAS_BEMF
} state_t;


typedef void (*state_fn)(void);

/*
state_fn state_functions[] = {
  dummy_fn,
  idle_fn,
  start_fn,
  check_feedback_fn,
  update_commutation_fn,
  setup_bemf_fn,
  meas_bemf_fn
};
*/

typedef enum {
  AUTO,
  ADC_READY,
  TIMER1A_INT,
  TIMER1B_INT
} event_t;


/*
typedef struct {
  state_fn transition_function;
  state_t next_state;
} transition_t;

transition_t transition_table[][] = {
  //AUTO               ADC_READY          TIMER1A_INT          TIMER1B_INT 
  {{,              IDLE,               IDLE,               IDLE               }, //IDLE
  {CHECK_FEEDBACK,     START,              START,              START              }, //START
  {UPDATE_COMMUTATION, CHECK_FEEDBACK,     CHECK_FEEDBACK,     CHECK_FEEDBACK     }, //CHECK_FEEDBACK
  {SETUP_BEMF,         UPDATE_COMMUTATION, UPDATE_COMMUTATION, UPDATE_COMMUTATION }, //UPDATE_COMMUTATION
  {SETUP_BEMF,         MEAS_BEMF,          SETUP_BEMF,         SETUP_BEMF         }, //SETUP_BEMF
  {MEAS_BEMF,          MEAS_BEMF,          CHECK_FEEDBACK,     MEAS_BEMF          }, //MEAS_BEMF
};
*/

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



event_t get_event();
state_t get_transition(char transition_table[][3], byte num_rows, state_t state, event_t event);

#endif //FSM_H_

/*

///////////////////////////////////////////////////////////////////
//http://www.pezzino.ch/state-machine-in-c/
typdef enum {
  STATE_A,
  STATE_B,
  STATE_C
} state_t;

typdef enum {
  EVENT_1,
  EVENT_2,
  EVENT_3
} event_t;

typedef void (*tfp)();

typdef struct {
  state_t next_state;
  tfp action;
} transition_t;

extern transition transition_table[NUM_STATES][NUM_EVENTS];

/////////////////////////////////////////////////////////////////

//https://www.embedded.com/design/prototyping-and-development/4442212/1/Implementing-finite-state-machines-in-embedded-systems

typdef enum {
  STATE_A,
  STATE_B,
  STATE_C,
  MAX_STATE
} state_t;

typdef enum {
  EVENT_1,
  EVENT_2,
  EVENT_3,
  MAX_STATE
} event_t;

state_t (*state_table[MAX_STATES][MAX_EVENTS])(void) = {
  {a,b,c},
  {d,e,f},
};



state = STATE_A;
while(1){
  event = read_event();
  if((event >= 0) && (event < MAX_EVENTS)){
    next_State = state_table[state][event]();
    state=next_state;
  }
 }


/////////////////////////////////////////////////////////////////
//https://aticleworld.com/state-machine-using-c/

typdef enum {
  STATE_A,
  STATE_B,
  STATE_C,
  LAST_STATE
} state_t;

typdef enum {
  EVENT_1,
  EVENT_2,
  EVENT_3,
  LAST_EVENT
} event_t;

//event_handler_fp is a typedef:
//  function pointer which takes no args and returns a state_t type.
//All my event handlers/state functions would be of this type.
//ie. take no args and return the next state
typdef state_t (*event_handler_fp)(void);

//event_handler_fa is a 2d array of funciton pointers which take no args and return a state_t
//ie. this is an array of event_handler_fp's.
//Can I just define it as that?
typedef state_t (*const event_handler_fa[LAST_STATE][LAST_EVENT])(void);

//example function call
state_t start_motor(void){
  //do the actual motor starting
  //mot.start(speed);
  return STATE_B;
}

//main loop
state_t next_state = STATE_A;
event_t new_event;


*/
