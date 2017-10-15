#include "adc.h"

byte analog_inputs[] = {VIN, VA, VB, VC, SO1, SO2};
byte current_adc_input;
unsigned int adc_vals[NUM_ADC_INPUTS];
byte new_adc_val;


/* Intially set up to require a manual trigger (ie. calling trigger_adc(chan)
 * During zero-crossing detection state, it should be changed to trigger
 * on overflow of timer/counter0. This will be in the middle of the "ON" pulse
 * of the pwm when (hopefully) everything is stable(ish)
 * 
 * once zero crossing is detected it should be switched back to manual mode
 * to do current/Vin/whatever else measurements as needed.
 *
 * Frequency is set based on value set in constants.h (ADC_PRESCALER)
 * I recomment ADC_PRESCALER = 32:
 * http://www.openmusiclabs.com/learning/digital/atmega-adc/
 * this seems to give still >9 bits of resolution and a max sampling rate of
 *  38Khz which is > my max PWM frequency of ~32kHz. With a sample/hold time of
 *  1.5 clock cycles = 3us, this should be small enough for all but the smallest
 *  duty cycles (in which case the measurement could just be a bit noisy).
 *
 */
void setup_adc(){
  current_adc_input = 0;
  new_adc_val = 0;

  ADMUX = _BV(REFS0);               //AVCC reference, right justified result, default input mux 0
  //enable ADC and set prescaler to 128 (16MHz/128 = 125kHz / 13 ~ 9615Hz = 104us
  ADCSRA = _BV(ADEN) | ADCSRA_MASK;
  ADCSRB = _BV(ADTS2);      //comparator disabled, auto-trigger source set to timer/counter0 overflow (but disabled in ADCSRA
  
  //The first conversion takes longer than normal, so trigger a conversion now
  trigger_adc(current_adc_input);
  while (!(ADCSRA & _BV(ADIF))){ }  //wait until the ADIF bit is set
  ADCSRA &= ~_BV(ADIF);             //clear the ADIF bit
  ADCSRA |= _BV(ADIE);              //enable the interrupt now that the first conversion is finished
}


/* Sets the ADC input channel-mul to 'channel'
 * Triggers a conversion to start
 */
void trigger_adc(byte channel){
  //Channel select is 4 LSB of ADMUX
  ADMUX = (ADMUX & 0b11110000) | (0b00001111 & channel);
  ADCSRA |= _BV(ADSC);
}


/* Sets the ADC input channel-mul to 'channel'
 * Triggers a conversion to start
 */
void set_adc_channel(byte channel){
  current_adc_input = channel;
  //Channel select is 4 LSB of ADMUX
  ADMUX = (ADMUX & 0b11110000) | (0b00001111 & channel);
}  


/*Set to auto trigger on timer/counter0 overflow*/
void set_auto_timer0_trigger(){
  ADCSRB |= _BV(ADTS2);   //set trigger source to timer/counter0 overflow
  ADCSRA |= _BV(ADATE);   //enable auto-trigger
}


/*Set into manual trigger mode*/
void set_manual_trigger(){
  ADCSRA &= ~_BV(ADATE);   //disable auto-trigger
}


/* ADC convers complete interrupt
 * saves the value into the adc_vals array
 */
ISR(ADC_vect){
  adc_vals[current_adc_input] = ((unsigned int)ADCL) | ((unsigned int)ADCH<<8);
  new_adc_val = 1;
}
