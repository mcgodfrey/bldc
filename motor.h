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

    MotorState state;       // Overall motor state
    Mode mode;              // Control mode (open/closed loop, feedback method)
    Direction direction;    // unused
    unsigned int target_speed;              // Speed in steps/second
    unsigned long target_phase_time;              // Speed in steps/second
    byte pmw_signal;
    byte commutation_state;
    
    unsigned long prev_phase_timestamp = 0;
    byte pwm_signal;        // PWM signal to use. 0-255. This is hard coded for open loop, or set by PI controller for closed loop
    
    // Called periodically to check the commutation state
    // This can be either open loop (ie. it just waits for the correct time as determined
    //  from the target speed), or closed loop, in which case it looks at the backEMF on
    //  the non-driven phase to look for a zero crossing.
    void check_commutation_state();

    // Take the current commutation_state and speed and update the PWM pins
    void update_commutation_state();
    
    void change_speed_relative(int change);

    void set_target_phase_time(unsigned long t);
    unsigned long get_target_phase_time();

};

#endif //_MOTOR_H
