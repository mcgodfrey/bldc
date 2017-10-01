#ifndef CONSTANTS_H_
#define CONSTANTS_H_


/*****
   Pins
   PWM pin mapping
   OC0A = 6
   OC0B = 5
   OC1A = 9
   OC1B = 10
   OC2A = 11
   OC2B = 3
 *****/
//Digital outputs
#define EN_GATE 10
#define INH_A 9
#define INH_B 8
#define INH_C 7
#define INL_A 11
#define INL_B 6
#define INL_C 5
//Digital inputs
#define nOCTW 2
#define nFAULT 3
//Analog inputs
#define VIN 0
#define VA 1
#define VB 2
#define VC 3
#define SO1 4
#define SO2 5
#define NUM_ADC_INPUTS 5


//Timer1 settings
#define SYSTEM_CLOCK_FREQ 16000000
#define TIMER1_PRESCALER 256

// ADC timer delay between triggers
// ADC conversion takes 100us. So I want to trigger every ~150-200us
//Timer1 should trigger every ~500us to update the state
#if TIMER1_PRESCALER == 1
#define TIMER1_PRESCALER_MASK (_BV(CS10))
#define ADC_DELAY (2400)
#define TIMER1_DELAY (10000)
#elif TIMER1_PRESCALER == 8
#define TIMER1_PRESCALER_MASK (_BV(CS11))
#define ADC_DELAY (300)
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

// PWM duty cycle (0-255) to use for open loop control
#define OPEN_LOOP_PWM 100

#define MAX_SPEED 100

#endif  //CONSTANTS_H_
