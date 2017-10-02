/*
 * BLDC motor object
 * 
 */

#ifndef _MOTOR_H
#define _MOTOR_H

#include <Arduino.h>
#include "constants.h"
#include "pwm.h"
#include "timer1.h"

//Definitions
enum Direction {FORWARD, BACKWARD};
enum MotorState {OFF, ON, COAST};

enum Mode {OpenLoop, BackEMF};


class Motor{
  public:
    Motor();

    void start();
    void stop();
    void check_commutation();
    // Called periodically to check the commutation state
    // This can be either open loop (ie. it just waits for the correct time as determined
    //  from the target speed), or closed loop, in which case it looks at the backEMF on
    //  the non-driven phase to look for a zero crossing.
    void check_commutation_state();
    // Take the current commutation_state and speed and update the PWM pins
    void update_commutation_state();

    /*Speed Changes*/
    void change_speed_relative(float change);
    void set_target_speed(float speed);
    float get_target_speed();


    MotorState state;       // Overall motor state
    Mode mode;              // Control mode (open/closed loop, feedback method)
    Direction direction;    // unused
    byte commutation_state;
    
    // private:
    // speed in rotations/second
    float target_speed;
    // Number of clock cycles per phase commutation. calculated from the target speed
    unsigned long prev_phase_timestamp = 0;
    
    
};

#endif //_MOTOR_H
