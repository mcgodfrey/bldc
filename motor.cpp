#include "motor.h"

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
  target_speed = 0;
  target_phase_time = 0;
  direction = FORWARD;
  digitalWrite(EN_GATE, HIGH);
  check_commutation();
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


void Motor::change_speed_relative(int change){
  if((change < 0) && (-1*change > target_speed)) {
    target_speed = 0;
  }else if((change > 0) && (MAX_SPEED-target_speed < change)){
    target_speed = MAX_SPEED;
  }else{
    target_speed += change;
  }
}


void Motor::set_target_phase_time(unsigned long t){
  target_phase_time = t;
}


unsigned long Motor::get_target_phase_time(){
  return(target_phase_time);
}


void Motor::check_commutation(){
  if(state == OFF){
    stop();
  }else if(mode == OpenLoop){
    //First hard code the pwm duty cycle
    pmw_signal = OPEN_LOOP_PWM;
    unsigned long current_timestamp = ((unsigned long)timer1_overflow<<16) + TCNT1;
    if((current_timestamp - prev_phase_timestamp) > target_phase_time){
      prev_phase_timestamp = current_timestamp;
      update_commutation_state();
    }
  }
}


void Motor::update_commutation_state(){
  commutation_state = (commutation_state+1)%6;
  switch(commutation_state){
    case 0:
      start_pwm(INH_A);
      stop_pwm(INH_B);
      stop_pwm(INH_C);
      digitalWrite(INL_A, HIGH);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, LOW);
      break;
    case 1:
      start_pwm(INH_A);
      stop_pwm(INH_B);
      stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, HIGH);
      digitalWrite(INL_C, LOW);
      break;
    case 2:
      stop_pwm(INH_A);
      start_pwm(INH_B);
      stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, HIGH);
      digitalWrite(INL_C, LOW);
      break;
    case 3:
      stop_pwm(INH_A);
      start_pwm(INH_B);
      stop_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, HIGH);
      break;
    case 4:
      stop_pwm(INH_A);
      stop_pwm(INH_B);
      start_pwm(INH_C);
      digitalWrite(INL_A, LOW);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, HIGH);
      break;
    case 5:
      stop_pwm(INH_A);
      stop_pwm(INH_B);
      start_pwm(INH_C);
      digitalWrite(INL_A, HIGH);
      digitalWrite(INL_B, LOW);
      digitalWrite(INL_C, LOW);
     break;
  }
}
