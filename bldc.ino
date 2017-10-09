/*
   Serial interface
   Based on example from here:
   https://learn.sparkfun.com/tutorials/lsm9ds0-hookup-guide/advanced-arduino-example

   back-emf BLDC control:
    https://www.digikey.com/en/articles/techzone/2013/jun/controlling-sensorless-bldc-motors-via-back-emf

   faster analog read (normally max 10kHz)
    https://forum.arduino.cc/index.php?topic=6549.0

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

byte accelerate;

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
/*
ISR(TIMER1_COMPB_vect){
  //select the channel and trigger the adc conversion
  current_adc_input = (current_adc_input+1)%NUM_ADC_INPUTS;
  trigger_adc(current_adc_input);
  //update OCR to trigger the next conversion
  OCR1B = TCNT1 + ADC_DELAY;
}
*/


/* Timer1 overflow should increment the global timer1_overflow variable
 * Turns my 16bit timer into a 32bit counter
 */
ISR(TIMER1_OVF_vect){
  timer1_overflow++;
  debug_led_state = (debug_led_state+1)%2;
  digitalWrite(LED_BUILTIN, debug_led_state);

  if(accelerate){
    float new_speed = (1+SPEED_MULTIPLIER) * mot.get_target_speed();
    if(new_speed < MAX_SPEED){
      mot.set_target_speed(new_speed);
    }else{
      mot.set_target_speed(MAX_SPEED);
      accelerate = 0;
    }
  }else{
    float new_speed = (1-SPEED_MULTIPLIER) * mot.get_target_speed();
    if(new_speed > MIN_SPEED){
      mot.set_target_speed(new_speed);
    }else{
      mot.set_target_speed(MIN_SPEED);
      accelerate = 1;
    }
  }
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


  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);


  
  //setup_menu();
  setup_pins();
  Serial.println("Set up pins");
  
  setup_pwm();
  Serial.println("Set up pwm");

  set_duty_cycle(pwm_duty_cycle);
  Serial.println("Set duty_cycle");

  setup_adc();
  Serial.println("Set up adc");

  current_adc_input = VC;
  set_auto_timer0_trigger();
  Serial.println("Set up adc trigger and input");

  setup_timer_1();
  Serial.println("Set up timer1");

  mot.start();
  Serial.println("Started motor");
}



/*
 * Arduino main loop
 * The main loop just handles the UI.
 * All the actual motor control is interrupt-driven for timing
 */
void loop() {
  //  if (Serial.available()) {
  //  m.parse_menu(Serial.read());
  // m.print_menu();
  //}
  //if (print_adc){
  //  print_adc = 0;
    //print_adc_vals();
  //}
  if(new_adc_val > 0){
    new_adc_val = 0;
    Serial.println(adc_vals[current_adc_input]);
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
