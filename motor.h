#ifndef MOTOR_H_
#define MOTOR_H_

#include "Arduino.h"

typedef enum {
  FORWARD,
  REVERSE
} direction_t;

typedef enum {
  ON,
  OFF
} motor_state_t;

typedef struct {
  float target_speed;
  byte commutation_state;
  direction_t direction;
  motor_state_t state;
  char undriven_phase;
} motor_t;


void motor_init(motor_t *m);
void motor_start(motor_t *m, float speed);
void motor_stop(motor_t *m);
void motor_update_commutation(motor_t *m);



#endif //MOTOR_H_
