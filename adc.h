#ifndef ADC_H_
#define ADC_H_

#include "Arduino.h"
#include "Constants.h"

extern unsigned int adc_vals[NUM_ADC_INPUTS];
extern byte new_adc_val;

typedef struct{
  byte interrupt;
} adc_t;


void adc_setup();
void adc_init(adc_t *a);
void adc_check_interrupt(adc_t *a);
void adc_trigger(byte channel);
void adc_set_channel(byte channel);
void adc_timer0_trigger();
void adc_disable_trigger();
void adc_clear_flag();


#endif //ADC_H_
