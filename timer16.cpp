#include "timer16.h"

static byte timer1a_flag;
static byte timer1b_flag;

void timer16_setup(){
  TCCR1A = 0;   //no PWM, normal mode
  TCCR1B = TIMER1_PRESCALER_MASK;
  TCCR1C = 0;   //unused
  TCNT1 = 0;   //timer initially 0
  timer1a_flag = 0;
  timer1b_flag = 0;
}

/* Initialise a timer object. There can be max 2. which = 'a' and 'b' */
void timer16_init(timer16_t *t, char which){
  t->which = which;
  t->interrupt = 0;
}

/* Reset the counter to 0 */
void timer16_reset(){
  TCNT1 = 0;
}

/* Must be called regularly to check if there was an interrupt */
void timer16_check_interrupt(timer16_t *t){
  if(t->which == 'a'){
    t->interrupt = timer1a_flag;
  } else if(t->which == 'b'){
    t->interrupt = timer1b_flag;
  }
}

/* Clear the interrupt flag after you've processed it */
void timer16_clear_interrupt(timer16_t *t){
  if(t->which == 'a'){
    timer1a_flag = 0;
  }else if(t->which == 'b'){
    timer1b_flag = 0;
  }
  t->interrupt = 0;
}

/*Set the timer1 compare interrupt to go off in "count" counts*/
void timer16_set_interrupt(timer16_t *t, unsigned int count){
  if(t->which == 'a'){
    OCR1A = TCNT1 + count;
    TIMSK1 |= _BV(OCIE1A);
    #ifdef DEBUG
    Serial.print("setting OCR1A to ");Serial.println(TCNT1+count);
    #endif
  }else if(t->which == 'b'){
    OCR1B = TCNT1 + count;
    TIMSK1 |= _BV(OCIE1B);
    #ifdef DEBUG
    Serial.print("setting OCR1B to ");Serial.println(TCNT1+count);
    #endif
  }
}

/* Disable interrupts */
void timer16_disable_interrupt(timer16_t *t){
  if(t->which == 'a'){
    TIMSK1 &= ~_BV(OCIE1A);
  }else if(t->which == 'b'){
    TIMSK1 &= ~_BV(OCIE1B);
  }
}


/* Just set the interrupt flag. nothing else. */
ISR(TIMER1_COMPA_vect){
  timer1a_flag = 1;
  #ifdef DEBUG
  Serial.println("Timer1A Triggered");
  #endif
}


/* Just set the interrupt flag. nothing else. */
ISR(TIMER1_COMPB_vect){
  timer1b_flag = 1;
  #ifdef DEBUG
  Serial.println("Timer1B Triggered");
  #endif
}

