/*
 * BLDC motor object
 * 
 * 
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

// PWM duty cycle (0-255) to use for open loop control
#define OPEN_LOOP_PWM 100

enum Direction {FORWARD, BACKWARD};
enum MotorState {OFF, ON, COAST};

enum Mode {OpenLoop, BackEMF};

enum CommutationState{
  State1 = 0,
  State2 = 1,
  State3 = 2,
  State4 = 3,
  State5 = 4,
  State6 = 5
}


class Motor{
  public:
    Motor();

    void start();
    void stop();

    MotorState state;       // Overall motor state
    Mode mode;              // Control mode (open/closed loop, feedback method)
    Direction direction;    // unused
    int target_speed;              // Speed in steps/second
    CommutationState commutation_state;

    char pwm_signal;        // PWM signal to use. 0-255. This is hard coded for open loop, or set by PI controller for closed loop

    // Called periodically to check the commutation state
    // This can be either open loop (ie. it just waits for the correct time as determined
    //  from the target speed), or closed loop, in which case it looks at the backEMF on
    //  the non-driven phase to look for a zero crossing.
    void check_commutation_state();

    // Take the current commutation_state and speed and update the PWM pins
    void update_commutation_state();
    
    void change_speed_relative(char change);

    int t_current_phase;
    int t_states[6];
    
    // Analog inputs (phase voltages and currents)
    int V1;
    int V2;
    int V3;
    int I1; 
    int I2;
 };

#endif
