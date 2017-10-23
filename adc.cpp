#include "adc.h"

byte current_adc_input;
unsigned int adc_vals[NUM_ADC_INPUTS];
byte new_adc_val;


void adc_setup(){
  current_adc_input = 0;
  new_adc_val = 0;

  //AVCC reference, right justified result, default input mux 0
  ADMUX = _BV(REFS0);
  //enable ADC and set prescaler to 128 (16MHz/128 = 125kHz / 13 ~ 9615Hz = 104us
  ADCSRA = _BV(ADEN) | ADCSRA_MASK;
  ADCSRB = _BV(ADTS2);      //comparator disabled, auto-trigger source set to timer/counter0 overflow (but disabled in ADCSRA
  
  //The first conversion takes longer than normal, so trigger a conversion now
  adc_trigger(current_adc_input);
  while (!(ADCSRA & _BV(ADIF))){ }  //wait until the ADIF bit is set
  ADCSRA &= ~_BV(ADIF);             //clear the ADIF bit
  ADCSRA |= _BV(ADIE);              //enable the interrupt now that the first conversion is finished
}

/* Sets the ADC input channel-mux to 'channel'
 * Triggers a conversion to start
 */
void adc_trigger(byte channel){
  adc_set_channel(channel);
  ADCSRA |= _BV(ADSC);
}

/* Sets the ADC input channel-mul to 'channel'
 * Triggers a conversion to start
 */
void adc_set_channel(byte channel){
  current_adc_input = channel;
  //Channel select is 4 LSB of ADMUX
  ADMUX = (ADMUX & 0b11110000) | (0b00001111 & channel);
}

/*Set to auto trigger on timer/counter0 overflow*/
void adc_timer0_trigger(){
  new_adc_val = 0;
  ADCSRB |= _BV(ADTS2);   //set trigger source to timer/counter0 overflow
  ADCSRA |= _BV(ADATE);   //enable auto-trigger
}


/*Disables auto-trigger.*/
void adc_disable_trigger(){
  ADCSRA &= ~_BV(ADATE);   //disable auto-trigger
}

void adc_clear_flag(){
  new_adc_val = 0;
}

/* ADC convers complete interrupt
 * saves the value into the adc_vals array
 */
ISR(ADC_vect){
  adc_vals[current_adc_input] = ADC;
  new_adc_val = 1;
}
