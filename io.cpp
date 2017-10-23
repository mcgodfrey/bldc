#include "io.h"

void io_setup(){
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

  //debugging
  pinMode(LED_BUILTIN, OUTPUT);
}


