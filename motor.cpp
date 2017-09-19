#include "motor.h"

Motor::Motor(){
  state = OFF;
  direction = FORWARD;
  commutation_state = 0;
  target_speed = 0;
}

void Motor::start(){
  state = ON;
  commutation_state = 0;
  target_speed = 0;
  target_phase_time = 0;
  direction = FORWARD;
  check_commutation();
}


void Motor::stop(){
  state = OFF;
  stop_pwm(EN1);
  stop_pwm(EN2);
  stop_pwm(EN3);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
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
    //unsigned long target_phase_time = 1000000/target_speed;  //in us
    unsigned long current_timestamp = ((unsigned long)timer1_overflow<<16) + TCNT1;
    //Serial.println("");
    //Serial.print("timer1_overflow = "); Serial.println(timer1_overflow);
    //Serial.print("current_timestamp = "); Serial.println(current_timestamp);
    //Serial.print("prev_phase_timestamp = "); Serial.println(prev_phase_timestamp);
    //Serial.print("target_phase_time = "); Serial.println(target_phase_time);
    if((current_timestamp - prev_phase_timestamp) > target_phase_time){
      //timer1_overflow = 0;
      prev_phase_timestamp = current_timestamp;
      update_commutation_state();
    }
  }
}


void Motor::update_commutation_state(){
  commutation_state = (commutation_state+1)%6;
  switch(commutation_state){
    case 0:
      start_pwm(EN1);
      stop_pwm(EN2);
      stop_pwm(EN3);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      break;
    case 1:
      start_pwm(EN1);
      stop_pwm(EN2);
      stop_pwm(EN3);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      break;
    case 2:
      stop_pwm(EN1);
      start_pwm(EN2);
      stop_pwm(EN3);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      break;
    case 3:
      stop_pwm(EN1);
      start_pwm(EN2);
      stop_pwm(EN3);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      break;
    case 4:
      stop_pwm(EN1);
      stop_pwm(EN2);
      start_pwm(EN3);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      break;
    case 5:
      stop_pwm(EN1);
      stop_pwm(EN2);
      start_pwm(EN3);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
     break;
  }
}
