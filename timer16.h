#ifndef _TIMER16_H
#define _TIMER16_H

#include <Arduino.h>
#include "constants.h"

typedef struct {
  char which;
  byte interrupt;
} timer16_t; 

void timer16_setup();
void timer16_init(timer16_t *t, char which);
void timer16_reset();
void timer16_check_interrupt(timer16_t *t);
void timer16_clear_interrupt(timer16_t *t);
void timer16_set_interrupt(timer16_t *t, unsigned int count);
void timer16_disable_interrupt(timer16_t *t);




#endif //_TIMER16_H
