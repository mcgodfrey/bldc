#include "fsm.h"
#include "timer16.h"
#include "adc.h"
#include "constants.h"

state_t current_state;
event_t event;
byte num_transitions;
timer16_t timer1a;
timer16_t timer1b;


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
  timer16_disable_interrupt(&timer1a);
  timer16_clear_interrupt(&timer1a);
  Serial.flush();
}

void update_commutation_fn(){
  Serial.println("UPDATE_COMMUTATION");
  timer16_reset();
  timer16_set_interrupt(&timer1a, 60000);  //Open loop timer
  timer16_set_interrupt(&timer1b, 1000);   //ADC noise delay
  Serial.flush();
}

void adc_noise_delay_fn(){
  Serial.println("ADC_NOISE_DELAY");
  timer16_disable_interrupt(&timer1b);
  timer16_clear_interrupt(&timer1b);
  Serial.flush();
}

void setup_vin_fn(){
  Serial.println("SETUP_VIN");
  adc_set_channel(ADC_VIN);
  adc_timer0_trigger();
  Serial.flush();
}

void meas_vin_fn(){
  adc_disable_trigger();
  adc_clear_flag();
  Serial.println("MEAS_VIN");
  Serial.print("VIN = ");Serial.println(adc_vals[ADC_VIN]);
  Serial.flush();
}

void setup_vref_fn(){
  Serial.println("SETUP_VREF");
  adc_set_channel(ADC_VREF);
  adc_timer0_trigger();
  Serial.flush();
}

void meas_vref_fn(){
  adc_disable_trigger();
  adc_clear_flag();
  Serial.println("MEAS_VREF");
  Serial.print("VREF = ");Serial.println(adc_vals[ADC_VREF]);
  Serial.flush();
}

void setup_bemf_fn(){
  Serial.println("SETUP_BEMF");
  adc_set_channel(ADC_VA);
  adc_timer0_trigger();
  Serial.flush();
}

void meas_bemf_fn(){
  adc_clear_flag();
  Serial.println("MEAS_BEMF");
  Serial.print("BEMF = ");Serial.println(adc_vals[ADC_VA]);
  Serial.flush();
}


state_fn state_functions[] = {
  idle_fn,
  start_fn,
  check_feedback_fn,
  update_commutation_fn,
  adc_noise_delay_fn,
  setup_vin_fn,
  meas_vin_fn,
  setup_vref_fn,
  meas_vref_fn,
  setup_bemf_fn,
  meas_bemf_fn
};

transition_t transition_table[] = {
  {IDLE,               AUTO,        START},
  {START,              AUTO,        CHECK_FEEDBACK},
  {CHECK_FEEDBACK,     AUTO,        UPDATE_COMMUTATION},
  {UPDATE_COMMUTATION, TIMER1B_INT, ADC_NOISE_DELAY},
  {ADC_NOISE_DELAY,    AUTO,        SETUP_VIN},
  {SETUP_VIN,          AUTO,        MEAS_VIN},
  {MEAS_VIN,           AUTO,        SETUP_VREF},
  {SETUP_VREF,         AUTO,        MEAS_VREF},
  {MEAS_VREF,          AUTO,        SETUP_BEMF},
  {SETUP_BEMF,         AUTO,        MEAS_BEMF},
  {MEAS_BEMF,          ADC_READY,   MEAS_BEMF},
  {MEAS_BEMF,          TIMER1A_INT, CHECK_FEEDBACK}
};



void setup(){
  Serial.begin(9600);
  current_state = IDLE;
  num_transitions = sizeof(transition_table)/(sizeof(transition_t));
  Serial.print("Num rows in transition table = ");Serial.println(num_transitions);
  timer16_setup();
  timer16_init(&timer1a, 'a');
  timer16_init(&timer1b, 'b');
  adc_setup();
}


void loop(){
  //  Serial.println(""); Serial.println("");
  //  Serial.print("current_state = ");Serial.println(current_state);
  event = get_event(&timer1a, &timer1b);
  //  Serial.print("event = ");Serial.println(event);
  state_t next_state = get_transition(transition_table, num_transitions, current_state, event);
  //  Serial.print("next_state = ");Serial.println(next_state);
  //  Serial.flush();
  if(next_state != NONE){
    state_functions[next_state]();
    current_state = next_state;
  }
  // }else{
  //   Serial.print("timer1a.interrupt = ");Serial.println(timer1a.interrupt);
  //   Serial.print(TCNT1);Serial.print("/");Serial.println(OCR1A);
  // }
}
