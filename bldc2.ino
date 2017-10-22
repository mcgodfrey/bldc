#include "fsm.h"


char transition_table[][3] = {
  {IDLE,               AUTO,        START},
  {START,              AUTO,        CHECK_FEEDBACK},
  {CHECK_FEEDBACK,     AUTO,        UPDATE_COMMUTATION},
  {UPDATE_COMMUTATION, AUTO,        SETUP_BEMF},
  {SETUP_BEMF,         ADC_READY,   MEAS_BEMF},
  {MEAS_BEMF,          ADC_READY,   MEAS_BEMF},
  {MEAS_BEMF,          TIMER1A_INT, CHECK_FEEDBACK}
};

void dummy_fn(){
  
}

void idle_fn(){
  Serial.println("IDLE");
  Serial.flush();
}

void start_fn(){
  Serial.println("START");
  Serial.flush();
}

void check_feedback_fn(){
  Serial.println("CHECK_FEEDBACK");
  Serial.flush();
}

void update_commutation_fn(){
  Serial.println("UPDATE_COMMUTATION");
  Serial.flush();
}

void setup_bemf_fn(){
  Serial.println("SETUP_BEMF");
  Serial.flush();
}

void meas_bemf_fn(){
  Serial.println("MEAS_BEMF");
  Serial.flush();
}


state_fn state_functions[] = {
  dummy_fn,
  idle_fn,
  start_fn,
  check_feedback_fn,
  update_commutation_fn,
  setup_bemf_fn,
  meas_bemf_fn
};


state_t current_state;
event_t event;
byte num_rows;

void setup(){
  Serial.begin(9600);
  current_state = IDLE;
  num_rows = sizeof(transition_table)/(3*sizeof(NONE));
  Serial.print("Num rows in transition table = ");Serial.println(num_rows);
}

void loop(){
  event = get_event();
  Serial.println("");  Serial.println("");
  Serial.print("current_state = ");Serial.println(current_state);
  Serial.print("event = ");Serial.println(event);
  state_t next_state = get_transition(transition_table, num_rows, current_state, event);
  Serial.print("next_state = ");Serial.println(next_state);
  Serial.flush();
  if(next_state != NONE){
    state_functions[next_state]();
    current_state = next_state;
  }
}
