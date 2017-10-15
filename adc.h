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
void set_adc_channel(byte channel);
void trigger_adc(byte channel);

#endif //_ADC_H
