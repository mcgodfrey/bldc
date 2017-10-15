#include "motor.h"
#include "adc.h"


Motor::Motor(){
  state = OFF;
  direction = FORWARD;
  commutation_state = 0;
  target_speed = 0;
  stop();
}

void Motor::start(){
  state = ON;
  commutation_state = 0;
  target_speed = INITIAL_SPEED;
  direction = FORWARD;
  digitalWrite(EN_GATE, HIGH);
  update_commutation();
}


void Motor::stop(){
  state = OFF;
  digitalWrite(EN_GATE, LOW);
  stop_pwm(INH_A);
  stop_pwm(INH_B);
  stop_pwm(INH_C);
  digitalWrite(INL_A, LOW);
  digitalWrite(INL_B, LOW);
  digitalWrite(INL_C, LOW);
}


void Motor::change_speed_relative(float change){
  Serial.print("current speed = ");Serial.println(target_speed);
  Serial.print("change = ");Serial.println(change);
  target_speed = target_speed * change;
  Serial.print("new speed = ");Serial.println(target_speed);
}

void Motor::set_target_speed(float speed){
  target_speed = speed;
}

float Motor::get_target_speed(){
  return(target_speed);
}

/* Checks if we need to advance the motor.
 * Open Loop Mode:
 *   Looks whether enough time has elapsed since the last commutation
 *   Phase time = timer_frequency [/second] / speed [rotations/second] / 6 [steps/rotation] 
 *              = [ counts/step ]
 */
void Motor::check_commutation(){
  unsigned long target_phase_time = (unsigned long)(TIMER1_FREQ / target_speed / 6.0);
  if(state == OFF){
    stop();
  }else if(mode == OpenLoop){
    unsigned long current_timestamp = ((unsigned long)timer1_overflow<<16) + TCNT1;
    if((current_timestamp - prev_phase_timestamp) > target_phase_time){
      prev_phase_timestamp = current_timestamp;
      commutation_state = (commutation_state+1)%6;
      update_commutation();
    }
  }
}


void Motor::update_commutation(){
  switch(commutation_state){
    case 0:
      set_manual_trigger();
      //set_auto_timer0_trigger();
      start_pwm(INH_A);
      stop_pwm(INH_B);
      stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, HIGH);
      digitalWrite(INL_C, LOW);
      break;
    case 1:
      //set_manual_trigger();
      set_auto_timer0_trigger();
      start_pwm(INH_A);
      stop_pwm(INH_B);
      stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, HIGH);
      break;
    case 2:
      set_manual_trigger();
      //set_auto_timer0_trigger();
      stop_pwm(INH_A);
      start_pwm(INH_B);
      stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, HIGH);
      break;
    case 3:
      set_manual_trigger();
      //set_auto_timer0_trigger();
      stop_pwm(INH_A);
      start_pwm(INH_B);
      stop_pwm(INH_C);
      digitalWrite(INL_A, HIGH);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, LOW);
      break;
    case 4:
      //set_manual_trigger();
      set_auto_timer0_trigger();
      stop_pwm(INH_A);
      stop_pwm(INH_B);
      start_pwm(INH_C);
      digitalWrite(INL_A, HIGH);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, LOW);
      break;
    case 5:
      set_manual_trigger();
      //set_auto_timer0_trigger();
      stop_pwm(INH_A);
      stop_pwm(INH_B);
      start_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, HIGH);
      digitalWrite(INL_C, LOW);
     break;
  }
}
