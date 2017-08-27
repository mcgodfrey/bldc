/*
 * Serial interface
 * Based on example from here:
 * https://learn.sparkfun.com/tutorials/lsm9ds0-hookup-guide/advanced-arduino-example
 * 
 * back-emf BLDC control:
 *  https://www.digikey.com/en/articles/techzone/2013/jun/controlling-sensorless-bldc-motors-via-back-emf
 * 
 * faster analog read (normally max 10kHz)
 *  https://forum.arduino.cc/index.php?topic=6549.0
 *  
 *  
 *  Overview:
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
 *      
 *  TODO:
 *  - How do I do open loop control at the beginning?
 *  - how/when do I switch from open-loop to closed loop back-emf control?
 *    * The state-cahnge logic is all in calc_new_commutation_state.
 *      This function can either use the back-emf, or timing info (i'll have to add timing info)
 *      I will add a new enum variable called control-type (OPEN, CLOSED)
 *      When in OPEN, it changes commutation state after a fixed time
 *      Once it reaches a certain speed (or it can do it automatically by looking at the back-emf magnitude) 
 *        it can switch over to closed loop.
 *      For debugging, I can run it 100% open loop.
 *  
 *  Alternative:
 *    * pass phase voltages into a comparator and use a digital input instead.
 *    
 *  Optional extras:
 *   * speed measurement
 *     - it needs to calculate time between phase state changes. Can just use millis()
 *     - should record the last few and average.
 *    
 *  Interrupts:
 *  ADC - 3 phase voltage measurement (for back-emf)
 *      - 2 current measurements
 *      
 * ADC interrupt logic
 *    - Set up V1 measurement
 *    - wait for conversion complete interrupt
 *    - save V1 to current state
 *    - set up V2 measurement
 *    - etc.
 *  
 *  
 *  
 */

#include "menu.h"
#include "motor.h"

MenuSystem m;
Motor mot;




void ADC_ISR(){
  switch(current_ADC_meas){
    case V1:
      motor.V1 = ADC_val;
      current_ADC_meas = V2;
      break;
    case V2:
      motor.V2 = ADC_val;
      current_ADC_meas = V3;
      break;
    case V3:
      motor.V3 = ADC_val;
      current_ADC_meas = I1;
      break;
    case I1:
      motor.I1 = ADC_val;
      current_ADC_meas = I2;
      break;
    case I2:
      motor.I2 = ADC_val;
      current_ADC_meas = V1;
      break;
  }
}

#define IN1 2
#define IN2 3
#define IN3 4
#define EN1 5
#define EN2 6
#define EN3 7
#define VM1 0
#define VM2 1
#define VM3 2
#define I1 3
#define I2 4


void speedUp(){
  Serial.println("Increasing the speed!!!");
}

void slowDown(){
  Serial.println("Slowing down...");
}

void start_motor(){
  Serial.println("Starting up!");
}

void stop_motor(){
  Serial.println("Stopping the motor");
}

void display_help(){
  Serial.println("Help menu:");
}


void setup() {
  Serial.begin(9600);

  //Set the PWM output pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);

  // Set the other pins
  pinMode(VM1, INPUT);
  pinMode(VM2, INPUT);
  pinMode(VM3, INPUT);
  pinMode(I1, INPUT);
  pinMode(VI2, INPUT);

  // Set the PWM frequency (eg. to 32kHz. see 
  // http://www.instructables.com/id/BLDC-Motor-Control-with-Arduino-salvaged-HD-motor/

  
  // Set up the Menu
  m.add_item(MenuItem('u', "Speed up", *speedUp));
  m.add_item(MenuItem('d', "Slow down", *slowDown));
  m.add_item(MenuItem('g', "Go", *start_motor));
  m.add_item(MenuItem('s', "Stop", *stop_motor));
  m.add_item(MenuItem('?', "Display Help", *display_help));
  m.print_menu();
}


void loop() {
  if(Serial.available()){
    m.parse_menu(Serial.read());
    m.print_menu();
  }

  //State comes from the motor position. ie. which phases need to be turned on.
  //I can get this from a hall sensor or from the back emf.
  byte state = calculate_state();

  // The desired speed. This could be an analog input or serial.
  // This should be where any sort of over-voltage/over-current/over-temperature
  // monitoring should happen. eg. it should throttle back the speed if the
  // current gets too high.
  byte spd = get_speed();
  
  // Now actually adjust the pwm outputs
  set_outputs(state, spd)
}

