#ifndef _PWM_H
#define _PWM_H

#include <Arduino.h>
#include "constants.h"

void set_duty_cycle(byte dc);
void setup_pwm();
void start_pwm(byte pin);
void stop_pwm(byte pin);



#endif //_PWM_H
