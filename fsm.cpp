#include "fsm.h"
#include "timer16.h"
#include "Arduino.h"

/*
 * Get the highest priority event
 * If nothing else, then return AUTO
 */
event_t get_event(timer16_t *timer1a, timer16_t *timer1b){
  timer16_check_interrupt(timer1a);
  timer16_check_interrupt(timer1b);
  if(timer1a->interrupt){
    return TIMER1A_INT;
  } else if(timer1b->interrupt){
    return TIMER1B_INT;
  } else if(0){ //if there is a new adc value
    return ADC_READY;
  } else {
    return AUTO;
  }
}


/*
 * Given the current state and new event, determine if there is a new state transition
 */
state_t get_transition(transition_t transition_table[], byte num_rows, state_t state, event_t event){
  //default to NONE
  state_t new_state = NONE;
  
  for(byte i = 0; i < num_rows; i++) {
    if(transition_table[i].current_state == state){
      if (transition_table[i].event == event){
        new_state = transition_table[i].new_state;
        break;
      }
    }
  }
  return(new_state);
}
