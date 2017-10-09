#ifndef _ADC_H
#define _ADC_H

#include <Arduino.h>
#include "constants.h"

extern byte analog_inputs[];
extern byte current_adc_input;
extern unsigned int adc_vals[NUM_ADC_INPUTS];
extern byte new_adc_val;

void setup_adc();
void set_auto_timer0_trigger();
void set_manual_trigger();


/* Sets the ADC input channel-mul to 'channel'
 * Triggers a conversion to start
 */
inline void trigger_adc(byte channel){
  //Channel select is 4 LSB of ADMUX
  ADMUX = (ADMUX & 0b11110000) | (0b00001111 & channel);
  ADCSRA |= _BV(ADSC);
}




#endif //_ADC_H
