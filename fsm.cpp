#include "fsm.h"
#include "Arduino.h"
#include "adc.h"
#include "timer16.h"


/*
 * Get the highest priority event
 * If nothing else, then return AUTO
 */
event_t get_event(config_t *c){
  timer16_check_interrupt(c->timer1a);
  timer16_check_interrupt(c->timer1b);
  adc_check_interrupt(c->adc);
  if(c->timer1a->interrupt){
    timer16_clear_interrupt(c->timer1a);
    return TIMER1A_INT;
  } else if(c->timer1b->interrupt){
    timer16_clear_interrupt(c->timer1b);
    return TIMER1B_INT;
  } else if(c->adc->interrupt){
    adc_clear_flag();
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
