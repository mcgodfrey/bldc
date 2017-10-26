#ifndef CONFIG_H_
#define CONFIG_H_

#include "Arduino.h"

typedef struct {
  byte open_loop_duty_cycle;
  float open_loop_speed;
  float open_loop_speed_hyst;
  float min_speed;
  float max_speed;
  float adc_noise_delay;
} config_t;


/*Load config from eeprom*/
void config_load(config_t *c);
/*Save config to eeprom*/
void config_save(config_t *c);

#endif //CONFIG_H_
