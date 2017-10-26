/*
 * PWM output module
 * Uses timer0 OCA and OCB, and timer2 OCA
 *   timer2 OCB is unused and can be used elsewhere (don't change anything else though)
 * Runs at a fixed ~32khz frequency (max speed) in phase correct mode
 * Both timers count together (set at almost the same time, as close as possible
 *   This is important since I trigger the adc on the overflow of timer0
 *   If the timers aren't in sync, then it is non-deterministic as to what
 *    timer2 is doing when the adc is measuring.
 * Duty cycle is set to be the same on all 3 outputs at the same time.
 * Duty cycle is a byte (0-255)
 *
 * The 3 channels are selected by passing in the corresponding arduino pin number.
 *  This is the "channel" input to start/stop pwm functions
 * These are defined in io.h as:
 * timer0 OCA: INH_B = 6
 * timer0 OCB: INH_C = 5
 * timer2 OCA: INH_A = 11
 */

#ifndef _PWM_H
#define _PWM_H

#include <Arduino.h>
#include "constants.h"

void pwm_setup();
void pwm_set_duty_cycle(byte duty_cycle);
void pwm_start(byte channel);
void pwm_stop(byte channel);



#endif //_PWM_H
