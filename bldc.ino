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
#include "pwm.h"
#include "timer1.h"
#include "motor.h"
#include "adc.h"


Motor mot;
MenuSystem m;
byte pwm_duty_cycle;

byte debug_led_state;
byte print_adc;

void setup_pins() {
  //Set the PWM output pins
  pinMode(INH_A, OUTPUT);
  pinMode(INH_B, OUTPUT);
  pinMode(INH_C, OUTPUT);
  //And the LOW output pins
  pinMode(INL_A, OUTPUT);
  pinMode(INL_B, OUTPUT);
  pinMode(INL_C, OUTPUT);

  //setup the digital IO
  pinMode(EN_GATE, OUTPUT);
  pinMode(nOCTW, INPUT);
  pinMode(nFAULT, INPUT);

  // Set the ADC input pins
  pinMode(VIN, INPUT);
  pinMode(VA, INPUT);  
  pinMode(VB, INPUT);
  pinMode(VC, INPUT);
  pinMode(SO1, INPUT);
  pinMode(SO2, INPUT);

  //debugging
  pinMode(LED_BUILTIN, OUTPUT);
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
 * Turns my 16bit timer into a 32bit counter
 */
ISR(TIMER1_OVF_vect){
  timer1_overflow++;
  debug_led_state = (debug_led_state+1)%2;
  digitalWrite(LED_BUILTIN, debug_led_state);
}


/* Timer1 COMPA is the main motor commutation timer
 * every time it expires we check and update commutation
 */
ISR(TIMER1_COMPA_vect){
  //set up the new interrupt output compare register value
  OCR1A = TCNT1 + TIMER1_DELAY;
  //update the commutation
  mot.check_commutation();
}


void setup_menu(){
  // Set up the Menu
  m.add_item(MenuItem('g', "Go", *start_motor));
  m.add_item(MenuItem('a', "Print ADC", *print_adc_vals));
  m.add_item(MenuItem('u', "Speed up", *speedUp));
  m.add_item(MenuItem('d', "Slow down", *slowDown));
  m.add_item(MenuItem('h', "Higher current", *increase_pwm));
  m.add_item(MenuItem('l', "Lower current", *decrease_pwm));
  m.add_item(MenuItem('s', "Stop", *stop_motor));
  m.add_item(MenuItem('?', "Display Help", *display_help));
  m.print_menu();
}

/*
 * Main arduino setup() function
 * perform inital pin setup, etc. before passing on to main loop()
 */
void setup(){
  debug_led_state = 0;
  print_adc = 0;
  pwm_duty_cycle = INITIAL_DUTY_CYCLE;
  Serial.begin(9600);

  setup_menu();
  setup_pins();
  setup_adc();
  setup_pwm();
  set_duty_cycle(pwm_duty_cycle);
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
  if (print_adc){
    print_adc = 0;
    //print_adc_vals();
  }
}



void print_adc_vals(){
  Serial.print("VIN = ");Serial.println(adc_vals[VIN - A0]);
  Serial.print("VA = ");Serial.println(adc_vals[VA - A0]);
  Serial.print("VB = ");Serial.println(adc_vals[VB - A0]);
  Serial.print("VC = ");Serial.println(adc_vals[VC - A0]);
  Serial.print("SO1 = ");Serial.println(adc_vals[SO1 - A0]);
  Serial.print("SO2 = ");Serial.println(adc_vals[SO2 - A0]);
}
  

/*
 * UI Callbacks
 */
float speed_inc = 0.1;
byte duty_cycle_inc = 10;

void speedUp() {
  Serial.println("Increasing the speed!!!");
  Serial.print("Old speed = "); Serial.println(mot.get_target_speed());
  mot.change_speed_relative(1.0+speed_inc);
  Serial.print("Speed = "); Serial.println(mot.get_target_speed());
}


void slowDown() {
  Serial.println("Slowing down...");
  Serial.print("Old speed = "); Serial.println(mot.get_target_speed());
  mot.change_speed_relative(1.0-speed_inc);
  Serial.print("Speed = "); Serial.println(mot.get_target_speed());
}

void increase_pwm() {
  Serial.println("Increasing PWM duty cycle...");
  Serial.print("Old duty cycle = "); Serial.println(pwm_duty_cycle);
  if(pwm_duty_cycle > 254-duty_cycle_inc){
    pwm_duty_cycle = 254;
  }else{
    pwm_duty_cycle += duty_cycle_inc;
  }
  set_duty_cycle(pwm_duty_cycle);
  Serial.print("New duty cycle = "); Serial.println(pwm_duty_cycle);
}

void decrease_pwm() {
  Serial.println("Decreasing PWM duty cycle...");
  Serial.print("Old duty cycle = "); Serial.println(pwm_duty_cycle);
  if(pwm_duty_cycle <= duty_cycle_inc){
    pwm_duty_cycle = 1;
  }else{
    pwm_duty_cycle -= duty_cycle_inc;
  }
  set_duty_cycle(pwm_duty_cycle);
  Serial.print("New duty cycle = "); Serial.println(pwm_duty_cycle);
}

void start_motor() {
  Serial.println("Starting up!");
  mot.start();
}

void stop_motor() {
  Serial.println("Stopping the motor");
  mot.stop();
}

void display_help() {
  Serial.println("Help menu:");
}
