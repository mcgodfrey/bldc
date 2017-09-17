/*
   Serial interface
   Based on example from here:
   https://learn.sparkfun.com/tutorials/lsm9ds0-hookup-guide/advanced-arduino-example

   back-emf BLDC control:
    https://www.digikey.com/en/articles/techzone/2013/jun/controlling-sensorless-bldc-motors-via-back-emf

   faster analog read (normally max 10kHz)
    https://forum.arduino.cc/index.php?topic=6549.0


    Overview:
 *  * main loop just handles serial comms
 *    * If it sees anything on the input then it triggers the appropriate callback
 *    * Note that this will be interrupted at any time for ADC/state updates.
 *  * ADC is basically free-running with interrupts for ADC conversion complete cycles
 *    * It cycles through the 5 inputs (3x voltage, 2x current)
 *    * -> max sampling rate for all 5 ~2khz
 *    * If i want 100 samples per period -> max 200 RPS = 12000 RPM
 *    * But, there are 6 "electrical" cycles per physical cycle -> max 2000RPM (still fast enough!)
 *  * Motor state is held in the Motor class
 *    * ADC updates the analog values as they come in
 *    * Timer interrupt routinely (at ~5KHz to be faster than ADC?) to check state
 *      * Performs averaging/smoothing of analog inputs
 *      * Decides which of the 6 states it's in (ie. which phases to drive) and updates outputs accordingly
 *        * ie. sets the PWM output pins
 *      *

    TODO:
    - How do I do open loop control at the beginning?
    - how/when do I switch from open-loop to closed loop back-emf control?
 *    * The state-cahnge logic is all in calc_new_commutation_state.
        This function can either use the back-emf, or timing info (i'll have to add timing info)
        I will add a new enum variable called control-type (OPEN, CLOSED)
        When in OPEN, it changes commutation state after a fixed time
        Once it reaches a certain speed (or it can do it automatically by looking at the back-emf magnitude)
          it can switch over to closed loop.
        For debugging, I can run it 100% open loop.

    Alternative:
 *    * pass phase voltages into a comparator and use a digital input instead.

    Optional extras:
 *   * speed measurement
       - it needs to calculate time between phase state changes. Can just use millis()
       - should record the last few and average.

    Interrupts:
    ADC - 3 phase voltage measurement (for back-emf)
        - 2 current measurements

   ADC interrupt logic
      - Set up V1 measurement
      - wait for conversion complete interrupt
      - save V1 to current state
      - set up V2 measurement
      - etc.

*/

#include "menu.h"
#include "constants.h"

Motor mot;
MenuSystem m;
byte pwm_out;
byte commutation_state;
unsigned int speed_delay;

byte analog_inputs[] = {VM1, VM2, VM3, I1, I2};
byte current_adc_input;
int adc_vals[NUM_ADC_INPUTS];


void set_duty_cycle(byte dc) {
  OCR0A = dc;
  OCR0B = dc;
  OCR2A = dc;
}




void setup_pins() {
  //Set the PWM output pins
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);
  //And the LOW output pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);

  // Set the ADC input pins
  pinMode(VM1, INPUT);
  pinMode(VM2, INPUT);
  pinMode(VM3, INPUT);
  pinMode(I1, INPUT);
  pinMode(I2, INPUT);
}

/*
 * Timer1 is used for phase timing and ADC trigger timing
 * 16MHz clock
 * 16 bit timer
 * prescaler = 16 -> 1 timer count = 1us
 * -> overflow in 2^16us = 65.5ms
 * prescaler = 256 -> 1.04s overflow
 */
void setup_timer_1(){
  TCCR1A = 0;   //no PWM, normal mode
  TCCR1B = TIMER1_PRESCALER_MASK;
  TCCR1C = 0;   //unused
  TCNT1H = 0;   //timer initially 0
  TCNT1L = 0;   //timer initially 0
  OCR1AH = speed_delay >> 8;        //initial timer is the default speed_delay
  OCR1AL = speed_delay & 0x00FF;    //initial timer is the default speed_delay
  OCR1BH = ADC_DELAY >> 8;        //initial timer is the default speed_delay
  OCR1BL = ADC_DELAY & 0x00FF;    //initial timer is the default speed_delay
  TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B); //interrupt on output comprae A and B match
}


/*
 * TIMER1B is the ADC timer clock
 * triggers a new conversion every time it expires
 * loops through and measures each input in turn
 */
ISR(TIMER1_COMPB_vect){
  //select the channel and trigger the adc conversion
  current_adc_input = (current_adc_input+1)%NUM_ADC_INPUTS;
  trigger_adc(current_adc_input);
  //update OCR to trigger the next conversion
  OCR1B = TCNT1 + ADC_DELAY;
}


/* Timer1 overflow should increment the global timer1_overflow variable
 * Turns by 16bit timer into a 32bit counter
 */
ISR(TIMER1_OVF_vect){
  timer1_overflow++;
}


ISR(TIMER1_COMPA_vect){
  //set up the new interrupt output compare register value
  OCR1A = TCNT1 + speed_delay;

  //update the commutation
  commutation_state = (commutation_state+1)%6;
  switch(commutation_state){
    case 0:
      start_pwm(EN1);
      stop_pwm(EN2);
      stop_pwm(EN3);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      break;
    case 1:
      start_pwm(EN1);
      stop_pwm(EN2);
      stop_pwm(EN3);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      break;
    case 2:
      stop_pwm(EN1);
      start_pwm(EN2);
      stop_pwm(EN3);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      break;
    case 3:
      stop_pwm(EN1);
      start_pwm(EN2);
      stop_pwm(EN3);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      break;
    case 4:
      stop_pwm(EN1);
      stop_pwm(EN2);
      start_pwm(EN3);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      break;
    case 5:
      stop_pwm(EN1);
      stop_pwm(EN2);
      start_pwm(EN3);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
     break;
  }
}

/* ADC convers complete interrupt
 * saves the value into the adc_vals array
 */
ISR(ADC_vect){
  adc_vals[current_adc_input] = ADCL | ADCH<<8;
}


/* Sets the ADC input channel-mul to 'channel'
 * Triggers a conversion to start
 */
inline void trigger_adc(byte channel){
  //Channel select is 4 LSB of ADMUX
  ADMUX = (ADMUX & 0b11110000) | (0b00001111 & channel);
  ADCSRA |= _BV(ADSC);
}


void setup_adc(){
  //initialise the adc_vals to something recognisable
  for(byte i = 0; i < NUM_ADC_INPUTS; i++) {
    adc_vals[i] = i*10;
  }
  ADMUX = _BV(REFS0);               //AVCC reference, right justified result, default input mux 0
  //enable ADC and set prescaler to 128 (16MHz/128 = 125kHz / 13 ~ 9615Hz = 104us
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
  ADCSRB = 0;                       //comparator disabled, auto-trigger source set to 0
  
  //The first conversion takes longer than normal, so trigger a conversion now
  trigger_adc(0);
  while (!(ADCSRA & _BV(ADIF))){ }  //wait until the ADIF bit is set
  ADCSRA &= ~_BV(ADIF);             //clear the ADIF bit
  ADCSRA |= _BV(ADIE);              //enable the interrupt now that the first conversion is finished
}


/* Set up Timer0 and Timer2 as phase correct PWM outputs
 * Maximum frequency (~32KHz)
 * All PWM outputs initially disabled
 */
void setup_pwm() {
  TCCR0A = _BV(WGM00);              //phase correct pwm. Initially both pwm outputs disabled
  TCCR2A = _BV(WGM20);
  TCCR0B = _BV(CS00);               //clock prescaler = 1 -> 16MHz/(2**8)/2 = 32KHz
  TCCR2B = _BV(CS20);
  TCNT0 = 0;                        //reset the timers
  TCNT2 = 0;                        //reset the timers

}


/* Start PWM output on a given pin (EN1, EN2 or EN3)
 * must be called once for each pin you want to start
 * setup_pwm() must be called first
 */
void start_pwm(byte pin){
  if(pin == EN1){
    TCCR2A |= _BV(COM2A1);
  }else if(pin == EN2){
    TCCR0A |= _BV(COM0A1);
  }else if(pin == EN3){
    TCCR0A |= _BV(COM0B1);
  }
}


/* Stop PWM output on a given pin (EN1, EN2 or EN3) and set it to LOW
 * must be called once for each pin you want to start
 * setup_pwm() must be called first
 */
void stop_pwm(byte pin){
  if(pin == EN1){
    TCCR2A &= ~(_BV(COM2A1));
    digitalWrite(EN1, LOW);
  }else if(pin == EN2){
    TCCR0A &= ~(_BV(COM0A1));
    digitalWrite(EN2, LOW);
  }else if(pin == EN3){
    TCCR0A &= ~(_BV(COM0B1));
    digitalWrite(EN3, LOW);
  }
}


void setup_menu(){
  // Set up the Menu
  m.add_item(MenuItem('u', "Speed up", *speedUp));
  m.add_item(MenuItem('d', "Slow down", *slowDown));
  m.add_item(MenuItem('g', "Go", *start_motor));
  m.add_item(MenuItem('s', "Stop", *stop_motor));
  m.add_item(MenuItem('a', "Print ADC", *print_adc));
  m.add_item(MenuItem('?', "Display Help", *display_help));
  m.print_menu();
}

/*
 * Main arduino setup() function
 * perform inital pin setup, etc. before passing on to main loop()
 */
void setup(){
  Serial.begin(9600);


  commutation_state = 0;
  current_adc_input = 0;

  setup_menu();
  setup_pins();
  setup_adc();
  setup_pwm();
  set_duty_cycle(20);
  speed_delay = 30000;
  setup_timer_1();

}



/*
 * Arduino main loop
 * The main loop just handles the UI.
 * All the actual motor control is interrupt-driven for timing
 */
void loop() {
  if (Serial.available()) {
    m.parse_menu(Serial.read());
    m.print_menu();
  }
}



/*
 * UI Callbacks
 */
void speedUp() {
  if (speed_delay < 20000) {
    speed_delay = 10000;
  } else {
    speed_delay -= 10000;
  }
  Serial.println("Increasing the speed!!!");
  Serial.print("Speed = ");
  Serial.println(speed_delay);
}

void slowDown() {
  if (speed_delay >  50000) {
    speed_delay = 60000;
  } else {
    speed_delay += 10000;
  }
  Serial.println("Slowing down...");
  Serial.print("Speed = ");
  Serial.println(speed_delay);
}

void start_motor() {
  Serial.println("Starting up!");
}

void stop_motor() {
  Serial.println("Stopping the motor");
}

void display_help() {
  Serial.println("Help menu:");
}

void print_adc() {
  Serial.println("ADC vals:");
  for(byte i = 0; i < NUM_ADC_INPUTS; i++){
    Serial.print(" "); Serial.print(i); Serial.print(" :");
    Serial.println(adc_vals[i]);
  }
}
