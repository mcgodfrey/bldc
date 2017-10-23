#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define DEBUG

//Timer1 settings
#define SYSTEM_CLOCK_FREQ 16000000
#define TIMER1_PRESCALER 256

#if TIMER1_PRESCALER == 1
#define TIMER1_PRESCALER_MASK (_BV(CS10))
#define ADC_DELAY (2400)
#define TIMER1_DELAY (10000)
#elif TIMER1_PRESCALER == 8
#define TIMER1_PRESCALER_MASK (_BV(CS11))
#define ADC_DELAY (3b00)
#define TIMER1_DELAY (1200)
#elif TIMER1_PRESCALER == 64
#define TIMER1_PRESCALER_MASK (_BV(CS11) | _BV(CS10))
#define ADC_DELAY (38)
#define TIMER1_DELAY (150)
#elif TIMER1_PRESCALER == 256
#define TIMER1_PRESCALER_MASK (_BV(CS12))
#define ADC_DELAY (10)
#define TIMER1_DELAY (44)
#elif TIMER1_PRESCALER == 1024
#define TIMER1_PRESCALER_MASK (_BV(CS12) | _BV(CS10))
#define ADC_DELAY (3)
#define TIMER1_DELAY (15)
#endif

#define TIMER1_FREQ ((unsigned long)((SYSTEM_CLOCK_FREQ)/(TIMER1_PRESCALER)))
#define TIMER1_MAX 65535


///////////////////////////////////////////////////////
//ADC

//ADC clock prescaler
#define ADC_PRESCALER 16
#define ADC_FREQUENCY ((unsigned long)(SYSTEM_CLOCK_FREQ/ADC_PRESCALER))
#if ADC_PRESCALER == 128
#define ADCSRA_MASK (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0))
#elif ADC_PRESCALER == 64
#define ADCSRA_MASK (_BV(ADPS2) | _BV(ADPS1))
#elif ADC_PRESCALER == 32
#define ADCSRA_MASK (_BV(ADPS2) | _BV(ADPS0))
#elif ADC_PRESCALER == 16
#define ADCSRA_MASK (_BV(ADPS2))
#elif ADC_PRESCALER == 8
#define ADCSRA_MASK (_BV(ADPS1) | _BV(ADPS0))
#elif ADC_PRESCALER == 4
#define ADCSRA_MASK (_BV(ADPS1))
#elif ADC_PRESCALER == 2
#define ADCSRA_MASK (_BV(ADPS0))
#endif

//ADC Inputs
#define NUM_ADC_INPUTS 16

#define ADC_VIN 0
#define ADC_VC 1
#define ADC_VB 2
#define ADC_VA 3
#define ADC_SO1 4
#define ADC_SO2 5
#define ADC_VREF 14
#define ADC_GND 15

//Work out these...
#define GET_VADC (adc_vals[ADC_VREF]*1024)
#define GET_VIN(ch) ((VREF/1024) * (float)adc_vals[ch])




// Board level constants
#define VREF 3.3
#define VDIV_R1 22000
#define VDIV_R2 10000
#define RSENSE 0.002
#define ISENSE_GAIN 40


#endif //CONSTANTS_H_
