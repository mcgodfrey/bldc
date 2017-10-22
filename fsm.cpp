#include "fsm.h"

#include "Arduino.h"
/*
 * Get the highest priority event
 * If nothing else, then return AUTO
 */
event_t get_event(){
  if(0){ //if there is a timer1a overflow
    return TIMER1A_INT;
  } else if(0){ //if there is a timer1b overflow
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
state_t get_transition(char transition_table[][3], byte num_rows, state_t state, event_t event){
  //default to NONE
  state_t new_state = NONE;
  
  for(byte i = 0; i < num_rows; i++) {
    Serial.print("    current row state = "); Serial.println(transition_table[i][0]);
    if((transition_table[i][0] = state)){
      Serial.println("  Found the state!");
      if (transition_table[i][1] == event){
        Serial.println("   Found the event!!");
        new_state = transition_table[i][2];
        break;
      }
    }
  }
  return(new_state);
}
