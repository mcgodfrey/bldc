#include "pwm.h"
#include "io.h"


/*Set up timers for whole chip*/
void pwm_setup() {
  TCCR0A = _BV(WGM00);    //phase correct pwm. Initially both pwm outputs disabled
  TCCR2A = _BV(WGM20);    // cont.
  TCCR0B = _BV(CS00);     //clock prescaler = 1 -> 16MHz/(2**8)/2 = 32KHz. Max frequency
  TCCR2B = _BV(CS20);     //cont
  TCNT0 = 0;              //reset the timers
  TCNT2 = 0;              //reset the timers
}


/*set duty cycle for all channels*/
void pwm_set_duty_cycle(byte duty_cycle){
  OCR0A = duty_cycle;
  OCR0B = duty_cycle;
  OCR2A = duty_cycle;
}

/*Start pwm on a single channel. 
 * Pin should be the arduino digio pin, defined in constants.h
 * One of INH_A, INH_B, INH_C.
 */
void pwm_start(byte pin){
  if(pin == INH_A){
    TCCR2A |= _BV(COM2A1);
  }else if(pin == INH_B){
    TCCR0A |= _BV(COM0A1);
  }else if(pin == INH_C){
    TCCR0A |= _BV(COM0B1);
  }
}

/*stop pwm on a single channel
 * Pin should be the arduino digio pin, defined in constants.h
 * One of INH_A, INH_B, INH_C.
 * Note, also actively output LOW on the pin with digitalWrite
 */
void pwm_stop(byte pin){
  if(pin == INH_A){
    TCCR2A &= ~(_BV(COM2A1));
    digitalWrite(INH_A, LOW);
  }else if(pin == INH_B){
    TCCR0A &= ~(_BV(COM0A1));
    digitalWrite(INH_B, LOW);
  }else if(pin == INH_C){
    TCCR0A &= ~(_BV(COM0B1));
    digitalWrite(INH_C, LOW);
  }
}
