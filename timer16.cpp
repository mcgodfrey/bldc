#include "timer16.h"

static byte timer1a_flag;
static byte timer1b_flag;
static unsigned int timer1a_cnt;

void timer16_setup(){
  TCCR1A = 0;   //no PWM, normal mode
  TCCR1B = TIMER1_PRESCALER_MASK;
  TCCR1C = 0;   //unused
  TIMSK1 = 0;
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

/* Calculates the time since timer=0 */
float timer16_get_time(){
  return((1.0*timer1a_cnt)/TIMER1_FREQ);
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

/*Set the timer1 compare interrupt to go off in "count" counts
 *Note that first I have to clear the corresponding output compare flag
 * Otherwise the interrupt will trigger immediately when you enable it
 * if the flag happens to be set. This is obviously not what we want...
 *Note I allow the TCNT1 + count addition to overflow. This is ok in this
 * case because the timer also overflows and should be correct. Since "count"
 * is the same size (16 bits) as the counter it will never be more than the
 * timer period.
*/
void timer16_set_interrupt(timer16_t *t, unsigned int count){
  if(t->which == 'a'){
    TIFR1 |= _BV(OCF1A);    //clear the compare flag
    OCR1A = TCNT1 + count;  //Set the compare register value
    TIMSK1 |= _BV(OCIE1A);  //Enable the interrupt
  }else if(t->which == 'b'){
    TIFR1 |= _BV(OCF1B);    //clear the compare flag
    OCR1B = TCNT1 + count;  //Set the compare register value
    TIMSK1 |= _BV(OCIE1B);  //Enable the interrupt
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
  timer1a_cnt = TCNT1;
  timer1a_flag = 1;
  #ifdef DEBUG
  Serial.print("Timer1A Triggered at ");Serial.println(TCNT1);
  #endif
}


/* Just set the interrupt flag. nothing else. */
ISR(TIMER1_COMPB_vect){
  timer1b_flag = 1;
  #ifdef DEBUG
  Serial.print("Timer1B Triggered at ");Serial.println(TCNT1);
  #endif
}

