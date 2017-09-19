#include "timer1.h"

unsigned int timer1_overflow;

/*
 * Timer1 is used for phase timing and ADC trigger timing
 * 16MHz clock
 * 16 bit timer
 * prescaler = 16 -> 1 timer count = 1us
 * -> overflow in 2^16us = 65.5ms
 * prescaler = 256 -> 1.04s overflow
 */
void setup_timer_1(){
  timer1_overflow = 0;

  TCCR1A = 0;   //no PWM, normal mode
  TCCR1B = TIMER1_PRESCALER_MASK;
  TCCR1C = 0;   //unused
  TCNT1H = 0;   //timer initially 0
  TCNT1L = 0;   //timer initially 0
  OCR1AH = TIMER1_DELAY >> 8;        //initial timer is the default speed_delay
  OCR1AL = TIMER1_DELAY & 0x00FF;    //initial timer is the default speed_delay
  OCR1BH = ADC_DELAY >> 8;        //initial timer is the default speed_delay
  OCR1BL = ADC_DELAY & 0x00FF;    //initial timer is the default speed_delay
 //interrupt on overflow, output comprae A and B match
  TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1);
}


