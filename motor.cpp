#include "motor.h"
#include "io.h"


void motor_init(motor_t *m){
  m->target_speed = 1;
  m->commutation_state = -1;
  m->undriven_phase = 'X';
}

void motor_update_commutation(motor_t *m){
  m->commutation_state = (m->commutation_state+1)%6;
  #ifdef DEBUG
  Serial.print("New commuation state = ");Serial.println(m->commutation_state);
  #endif
  switch(m->commutation_state){
    case 0:
      digitalWrite(INH_A, HIGH); //start_pwm(INH_A);
      digitalWrite(INH_B, LOW); //stop_pwm(INH_B);
      digitalWrite(INH_B, LOW); //stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, HIGH);
      digitalWrite(INL_C, LOW);
      m->undriven_phase = 'C';
      break;
    case 1:
      digitalWrite(INH_A, HIGH); //start_pwm(INH_A);
      digitalWrite(INH_B, LOW); //stop_pwm(INH_B);
      digitalWrite(INH_B, LOW); //stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, HIGH);
      m->undriven_phase = 'B';
      break;
    case 2:
      digitalWrite(INH_A, LOW); //stop_pwm(INH_A);
      digitalWrite(INH_B, HIGH); //start_pwm(INH_B);
      digitalWrite(INH_B, LOW); //stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, HIGH);
      m->undriven_phase = 'A';
      break;
    case 3:
      digitalWrite(INH_A, LOW); //stop_pwm(INH_A);
      digitalWrite(INH_B, HIGH); //start_pwm(INH_B);
      digitalWrite(INH_B, LOW); //stop_pwm(INH_C);
      digitalWrite(INL_A, HIGH);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, LOW);
      m->undriven_phase = 'C';
      break;
    case 4:
      digitalWrite(INH_A, LOW); //stop_pwm(INH_A);
      digitalWrite(INH_B, LOW); //stop_pwm(INH_B);
      digitalWrite(INH_B, HIGH); //start_pwm(INH_C);
      digitalWrite(INL_A, HIGH);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, LOW);
      m->undriven_phase = 'B';
      break;
    case 5:
      digitalWrite(INH_A, LOW); //stop_pwm(INH_A);
      digitalWrite(INH_B, LOW); //stop_pwm(INH_B);
      digitalWrite(INH_B, HIGH); //start_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, HIGH);
      digitalWrite(INL_C, LOW);
      m->undriven_phase = 'A';
     break;
  }
}
