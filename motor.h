#ifndef MOTOR_H_
#define MOTOR_H_

#include "Arduino.h"

typedef struct {
  float target_speed;
  byte commutation_state;
  char undriven_phase;
} motor_t;

void motor_init(motor_t *m);
void motor_update_commutation(motor_t *m);



#endif //MOTOR_H_
