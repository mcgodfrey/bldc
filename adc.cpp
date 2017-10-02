#include "adc.h"

byte analog_inputs[] = {VIN, VA, VB, VC, SO1, SO2};
byte current_adc_input;
unsigned int adc_vals[NUM_ADC_INPUTS];



void setup_adc(){
  current_adc_input = 0;
  ADMUX = _BV(REFS0);               //AVCC reference, right justified result, default input mux 0
  //enable ADC and set prescaler to 128 (16MHz/128 = 125kHz / 13 ~ 9615Hz = 104us
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
  ADCSRB = 0;                       //comparator disabled, auto-trigger source set to 0
  
  //The first conversion takes longer than normal, so trigger a conversion now
  trigger_adc(current_adc_input);
  while (!(ADCSRA & _BV(ADIF))){ }  //wait until the ADIF bit is set
  ADCSRA &= ~_BV(ADIF);             //clear the ADIF bit
  ADCSRA |= _BV(ADIE);              //enable the interrupt now that the first conversion is finished
}



/* ADC convers complete interrupt
 * saves the value into the adc_vals array
 */
ISR(ADC_vect){
  adc_vals[current_adc_input] = ((unsigned int)ADCL) | ((unsigned int)ADCH<<8);
}


