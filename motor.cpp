#include "motor.h"

Motor::Motor(){
  state = OFF;
  direction = FORWARD;
  commutation_state = State1;
  target_speed = 0;
}

Motor::start(){
  state = ON
  commutation_state = START_COMMUTATION_STATE;
  target_speed = 0;
  direction = FORWARD;
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


void Motor::check_commutation(){
  if(mode == OpenLoop){
    //First hard code the pwm duty cycle
    pmw_signal = OPEN_LOOP_PWM;

    //Open Loop is a speed controller. It just looks whether enough time
    // has passed since the previous commutation. If so, then it advances.
    // If not, then do nothing
    unsigned long target_phase_time = 1000000/target_speed;  //in us
    unsigned long current_time = micros()
    // If enough time has passed, then move on to the next commutation state
    unsigned long elapsed_time = current_time - prev_phase_time;
    if(elapsed_time > target_phase_time) {
      if(commutation_state = State6){
        commutation_state = State1;
      }else{
        commutation_state++;
      }
      prev_phase_time = current_time;
      update_commutation_state()
    }
  }
}


void Motor::update_commutation_state(){
  byte hi_pin;
  byte lo_pin;
  byte floating_pin;
  switch(commutation_state){
    case State1:
      hi_pin = EN1;
      lo_pin = EN2;
      break;
    case State2:
      hi_pin = EN1;
      lo_pin = EN2;
      break;
    case State3:
      hi_pin = EN1;
      lo_pin = EN2;
      break;
    case State4:
      hi_pin = EN1;
      lo_pin = EN2;
      break;
    case State5:
      hi_pin = EN1;
      lo_pin = EN2;
      break;
    case State6:
      hi_pin = EN1;
      lo_pin = EN2;
      break;
   }
   analogWrite(hi_pin, pwm_signal);
   analogWrite(lo_pin, 255);
   analogWrite(floating_pin, 0); //what do I do with this?
}

