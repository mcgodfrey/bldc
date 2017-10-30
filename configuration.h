#ifndef CONFIG_H_
#define CONFIG_H_

#include "Arduino.h"
#include "timer16.h"
#include "adc.h"
#include "motor.h"


typedef struct {
  //to keep track of what is saved in eeprom
  unsigned int config_version;

  //State variables
  byte open_loop;
  byte open_loop_duty_cycle;
  float open_loop_speed;
  float open_loop_speed_hyst;
  float min_speed;
  float max_speed;
  float current_speed;
  float target_speed;
  float max_acceleration;
  unsigned int adc_noise_delay;

  //objects
  timer16_t *timer1a;
  timer16_t *timer1b;
  adc_t *adc;
  motor_t *motor;
} config_t;


/*Load config from eeprom*/
void config_load(config_t *c);
/*Save config to eeprom*/
void config_save(config_t *c);
/*initialise config struct*/
void config_init(config_t *c, timer16_t *t1a, timer16_t *t1b, adc_t *adc, motor_t *mot);

#endif //CONFIG_H_
