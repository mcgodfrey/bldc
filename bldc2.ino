#include "fsm.h"
#include "timer16.h"
#include "adc.h"
#include "constants.h"
#include "io.h"
#include "pwm.h"
#include "motor.h"
#include "configuration.h"
#include "commands.h"


state_t current_state;
event_t event;
timer16_t timer1a;
timer16_t timer1b;
motor_t motor;
config_t config;
adc_t adc;

byte num_transitions;

/*SW delay for debugging since delay() doesn't work any more
 * The ms timing is only *very* approximate.
 * I should really do this with one of the counters
*/
void sw_delay(unsigned int ms){
  float delay_time = (0.001*ms*SYSTEM_CLOCK_FREQ)/100;
  for(unsigned long i=0; i<delay_time; i++){
    volatile int x = i+i;
  }
}

void flash_led(byte num_flashes){
  for(byte i=0; i<num_flashes; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    sw_delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    sw_delay(50);
  }
}

void dummy_fn(config_t *c){
  
}

void idle_fn(config_t *c){
  Serial.println(F("IDLE"));
  Serial.flush();
}

void start_fn(config_t *c){
  Serial.print("min_speed = "); Serial.println(c->min_speed);
  Serial.print("max_speed = "); Serial.println(c->max_speed);
  Serial.print("adc_noise_delay = "); Serial.println(c->adc_noise_delay);
  motor_start(c->motor);
  c->current_speed = c->min_speed;
  c->target_speed = c->min_speed;
  Serial.println(F("START"));
  Serial.flush();
}

void check_feedback_fn(config_t *c){
  adc_disable_trigger();
  timer16_disable_interrupt(c->timer1a);

  float prev_time = timer16_get_time();
  c->current_speed = 1.0/prev_time;
  
  //Check if we need to change between open/closed loop (based on speed)
  if((c->open_loop)
     && (c->current_speed > (c->open_loop_speed + c->open_loop_speed_hyst))){
    c->open_loop = 0;
  }else if((!c->open_loop)
           && (c->current_speed < (c->open_loop_speed - c->open_loop_speed_hyst))){
    c->open_loop = 1;
  }

}

unsigned int calc_open_loop_delay(config_t *c){
  float new_speed;
  new_speed = c->target_speed;
  if(new_speed > c->max_speed){
    new_speed = c->max_speed;
  }else if(new_speed < c->min_speed){
    new_speed = c->min_speed;
  }
  unsigned int delay_count = TIMER1_FREQ / new_speed;
  return(delay_count);
}

/*placeholder*/
byte calc_closed_loop_pwm(config_t *c){
  return(40);
}

void update_commutation_fn(config_t *c){
  motor_update_commutation(c->motor);
  timer16_reset();

  if(c->open_loop){
    unsigned int delay_count = calc_open_loop_delay(c);
    timer16_set_interrupt(c->timer1a, delay_count);  //Open loop timer
    timer16_set_interrupt(c->timer1b, c->adc_noise_delay); //ADC noise delay
  }else{
    byte pwm = calc_closed_loop_pwm(c);
    //set the pwm here
    timer16_reset();
    timer16_set_interrupt(c->timer1b, c->adc_noise_delay); //ADC noise delay
  }
}


void adc_noise_delay_fn(config_t *c){
  timer16_disable_interrupt(c->timer1b);
}

void setup_vin_fn(config_t *c){
  adc_set_channel(ADC_VIN);
  adc_timer0_trigger();
}

void meas_vin_fn(config_t *c){
  adc_disable_trigger();
  adc_clear_flag();
}

void setup_vref_fn(config_t *c){
  adc_set_channel(ADC_VREF);
  adc_timer0_trigger();
}

void meas_vref_fn(config_t *c){
  adc_disable_trigger();
  adc_clear_flag();
}

void setup_bemf_fn(config_t *c){
  adc_set_channel(ADC_VA);
  adc_timer0_trigger();
}

void meas_bemf_fn(config_t *c){
  adc_clear_flag();
  if(c->motor->undriven_phase == 'A'){
    Serial.println(adc_vals[ADC_VA]);
  }
  #ifdef DEBUG
  Serial.println(F("MEAS_BEMF"));
  Serial.print(F("BEMF = "));Serial.println(adc_vals[ADC_VA]);
  Serial.flush();
  #endif
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
  {START,              AUTO,        UPDATE_COMMUTATION},
  {CHECK_FEEDBACK,     AUTO,        UPDATE_COMMUTATION},
  {UPDATE_COMMUTATION, TIMER1B_INT, ADC_NOISE_DELAY},
  {ADC_NOISE_DELAY,    AUTO,        SETUP_VIN},
  {SETUP_VIN,          ADC_READY,   MEAS_VIN},
  {MEAS_VIN,           AUTO,        SETUP_VREF},
  {SETUP_VREF,         ADC_READY,   MEAS_VREF},
  {MEAS_VREF,          AUTO,        SETUP_BEMF},
  {SETUP_BEMF,         AUTO,        MEAS_BEMF},
  {MEAS_BEMF,          ADC_READY,   MEAS_BEMF},
  {MEAS_BEMF,          TIMER1A_INT, CHECK_FEEDBACK}
};



void set_speed(config_t *conf, const char *c){
  if(c[0] == '?'){
    Serial.print("Current Speed = ");Serial.println(conf->current_speed);
    //Serial.print("Max_dv = ");Serial.println(conf->max_dv);
  }else{
    float speed = atof(c);
    if(speed != 0.0){
      //#ifdef DEBUG
      Serial.print(F("Setting speed to "));Serial.println(speed);
      Serial.flush();
      //#endif
      conf->target_speed = speed;
    }else{
      Serial.println(F("Invalid speed"));
    }
  }
}

void set_motor_state(config_t *conf, const char *c){
  Serial.print(F("Setting motor state to "));Serial.println(c);
}

void set_pwm(config_t *conf, const char *c){
  byte pwm = atoi(c);
  Serial.print(F("Setting pwm to "));Serial.println(pwm);
}

rx_cmd_t rx_commands[] = {
  {"speed", set_speed},
  {"motor", set_motor_state},
  {"pwm", set_pwm}
};


void setup(){
  flash_led(10);
  Serial.begin(9600);
  byte num_commands = sizeof(rx_commands)/(sizeof(rx_cmd_t));
  rx_commands_init(rx_commands, num_commands, &config);
  
  config_init(&config, &timer1a, &timer1b, &adc, &motor);
  current_state = IDLE;
  num_transitions = sizeof(transition_table)/(sizeof(transition_t));
  timer16_setup();
  timer16_init(&timer1a, 'a');
  timer16_init(&timer1b, 'b');
  adc_setup();
  adc_init(&adc);
  io_setup();
  pwm_setup();
  pwm_set_duty_cycle(40);
  motor_init(&motor);
}


void loop(){
  event = get_event(&config);
  state_t next_state = get_transition(transition_table, num_transitions, current_state, event);
  if(next_state != NONE){
    state_functions[next_state](&config);
    current_state = next_state;
  } else {
    if(Serial.available() > 0){
      char c = Serial.read();
      rx_process_char(c);
    }
  }
}
