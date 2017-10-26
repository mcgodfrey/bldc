#include "motor.h"
#include "io.h"
#include "pwm.h"
#include "constants.h"


void motor_init(motor_t *m){
  m->target_speed = 1;
  m->commutation_state = 0;
  m->direction = FORWARD;
  m->state = OFF;
  m->undriven_phase = 'X';
}


void motor_start(motor_t *m, float speed){
  //first stop the motor to turn everything off and get into a known state
  motor_stop(m);
  // And now set everything up to turn on
  m->state = ON;
  m->commutation_state = 0;
  m->target_speed = speed;
  m->direction = FORWARD;
  digitalWrite(EN_GATE, HIGH);
}

void motor_stop(motor_t *m){
  m->state = OFF;
  digitalWrite(EN_GATE, LOW);
  pwm_stop(INH_A);
  pwm_stop(INH_B);
  pwm_stop(INH_C);
  digitalWrite(INL_A, LOW);
  digitalWrite(INL_B, LOW);
  digitalWrite(INL_C, LOW);
}


void motor_update_commutation(motor_t *m){
  m->commutation_state = (m->commutation_state+1)%6;
  //#ifdef DEBUG
  Serial.print("New commuation state = ");Serial.println(m->commutation_state);
  //#endif
  switch(m->commutation_state){
    case 0:
      pwm_start(INH_A); //digitalWrite(INH_A, HIGH); //
      pwm_stop(INH_B); //digitalWrite(INH_B, LOW); //
      pwm_stop(INH_C); //digitalWrite(INH_C, LOW); //
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, HIGH);
      digitalWrite(INL_C, LOW);
      m->undriven_phase = 'C';
      break;
    case 1:
      pwm_start(INH_A); //digitalWrite(INH_A, HIGH); //
      pwm_stop(INH_B); //digitalWrite(INH_B, LOW); //
      pwm_stop(INH_C); //digitalWrite(INH_C, LOW); //
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, HIGH);
      m->undriven_phase = 'B';
      break;
    case 2:
      pwm_stop(INH_A);
      pwm_start(INH_B);
      pwm_stop(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, HIGH);
      m->undriven_phase = 'A';
      break;
    case 3:
      pwm_stop(INH_A);
      pwm_start(INH_B);
      pwm_stop(INH_C);
      digitalWrite(INL_A, HIGH);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, LOW);
      m->undriven_phase = 'C';
      break;
    case 4:
      pwm_stop(INH_A);
      pwm_stop(INH_B);
      pwm_start(INH_C);
      digitalWrite(INL_A, HIGH);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, LOW);
      m->undriven_phase = 'B';
      break;
    case 5:
      pwm_stop(INH_A);
      pwm_stop(INH_B);
      pwm_start(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, HIGH);
      digitalWrite(INL_C, LOW);
      m->undriven_phase = 'A';
     break;
  }
}
