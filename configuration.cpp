#include "configuration.h"
#include "EEPROM.h"


/*Load config from eeprom*/
void config_load(config_t *c){
  //  EEPROM.get(0, *c);
  c->config_version = 1;
  c->open_loop = 1;       //initially open loop
  c->open_loop_speed = 1e6;
  c->open_loop_duty_cycle = 40;
  c->open_loop_speed_hyst = 20;
  c->min_speed = ((float)TIMER1_FREQ)/TIMER1_MAX;
  c->max_speed = ((float)TIMER1_FREQ)/100.0;
  c->max_acceleration = 10;
  c->adc_noise_delay = (ADC_DELAY*TIMER1_FREQ)/1e6;
}

/*Save config to eeprom*/
void config_save(config_t *c){
  //  EEPROM.put(0, *c);
}

void config_init(config_t *c, timer16_t *t1a, timer16_t *t1b, adc_t *adc, motor_t *mot){
  config_load(c);
  c->timer1a = t1a;
  c->timer1b = t1b;
  c->adc = adc;
  c->motor = mot;
}
