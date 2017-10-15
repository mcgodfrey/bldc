#ifndef CONSTANTS_H_
#define CONSTANTS_H_


//Initial settings
#define INITIAL_DUTY_CYCLE 30
#define INITIAL_SPEED 0.1

#define MIN_SPEED 0.1
#define MAX_SPEED 20
#define SPEED_MULTIPLIER 0.1

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
#define INH_A 11
#define INH_B 6
#define INH_C 5
#define INL_A 9
#define INL_B 8
#define INL_C 7
//Digital inputs
#define nOCTW 2
#define nFAULT 3
//Analog inputs
// Note that A0 starts at pin 14
#define VIN 0
#define VC 1
#define VB 2
#define VA 3
#define SO1 4
#define SO2 5
#define BG_REF 14
#define NUM_ADC_INPUTS 6


//Timer1 settings
#define SYSTEM_CLOCK_FREQ 16000000
#define TIMER1_PRESCALER 256

// ADC timer delay between triggers
// ADC conversion takes 100us. So I want to trigger every ~150-200us
// Timer1 should trigger every ~500us to update the state
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

//PWM clock prescaler
#define PWM_PRESCALER 1
#define PWM_FREQUENCY ((unsigned long)(SYSTEM_CLOCK_FREQ/PWM_PRESCALER/(2*256)))
#if PWM_PRESCALER == 1
#define PWM_CS_MASK (_BV(CS00))
#elif PWM_PRESCALER == 8
#define PWM_CS_MASK (_BV(CS01))
#elif PWM_PRESCALER == 64
#define PWM_CS_MASK (_BV(CS01) | _BV(CS00))
#elif PWM_PRESCALER == 256
#define PWM_CS_MASK (_BV(CS02))
#elif PWM_PRESCALER == 1024
#define PWM_CS_MASK (_BV(CS02) | _BV(CS00))
#endif


#endif  //CONSTANTS_H_
