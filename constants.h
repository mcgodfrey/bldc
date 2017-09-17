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
#define IN1 9
#define IN2 8
#define IN3 7
#define EN1 11
#define EN2 6
#define EN3 5
//Analog inputs
#define VM1 0
#define VM2 1
#define VM3 2
#define I1 3
#define I2 4
#define NUM_ADC_INPUTS 5


//Timer1 settings
#define TIMER1_PRESCALER 256

// ADC timer delay between triggers
// ADC conversion takes 100us. So I want to trigger every ~150-200us
#if TIMER1_PRESCALER == 1
#define TIMER1_PRESCALER_MASK (_BV(CS10))
#define ADC_DELAY (2400)
#elif TIMER1_PRESCALER == 8
#define TIMER1_PRESCALER_MASK (_BV(CS11))
#define ADC_DELAY (300)
#elif TIMER1_PRESCALER == 64
#define TIMER1_PRESCALER_MASK (_BV(CS11) | _BV(CS10))
#define ADC_DELAY (38)
#elif TIMER1_PRESCALER == 256
#define TIMER1_PRESCALER_MASK (_BV(CS12))
#define ADC_DELAY (10)
#elif TIMER1_PRESCALER == 1024
#define TIMER1_PRESCALER_MASK (_BV(CS12) | _BV(CS10))
#define ADC_DELAY (3)
#endif

// PWM duty cycle (0-255) to use for open loop control
#define OPEN_LOOP_PWM 100


#endif  //CONSTANTS_H_
