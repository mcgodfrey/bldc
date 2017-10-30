#ifndef IO_H_
#define IO_H_
#include "Arduino.h"

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


void io_setup(void);

#endif //IO_H_
