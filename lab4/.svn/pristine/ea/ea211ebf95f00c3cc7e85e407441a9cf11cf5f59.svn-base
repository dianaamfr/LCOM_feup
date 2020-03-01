#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

uint32_t TIMER_COUNTER = 0;

int TIMER_HOOK_ID = TIMER0_IRQ; 
/*before calling sys_irqsetpolicy the variable represents a identifier associated with a specific diver - timer 0 in this case*/
/*after calling sys_irqsetpolicy, hook_id is used to specifiy the  interrupt notification to operate on*/

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if(timer != 0 && timer!= 1 && timer != 2)
    return 1;

  if(freq < 19 || freq > TIMER_FREQ )
    return 1;

  uint8_t status;
  uint8_t lsb,msb;
  uint8_t control_word_4lsbits;
  uint8_t control_word;
  uint16_t divisor = (uint16_t)(TIMER_FREQ / freq);

  //get status and mantain counting base and operating mode
  if (timer_get_conf(timer, &status) == 0) {
    control_word_4lsbits = status & 0X0F;

    //build new Control Word
    if (timer == 0) {
      control_word = (TIMER_SEL0 | TIMER_LSB_MSB | control_word_4lsbits);
    }
    else if (timer == 1) {
      control_word = (TIMER_SEL1 | TIMER_LSB_MSB | control_word_4lsbits);
    }
    else if (timer == 2) {
      control_word = (TIMER_SEL2 | TIMER_LSB_MSB | control_word_4lsbits);
    }
    else
      return 1;

    uint8_t counter; //endereço do counter

    if (sys_outb(TIMER_CTRL, control_word) == OK) {
       if (timer == 0) {
        counter = TIMER_0;
      }
      else if (timer == 1) {
        counter = TIMER_1;
      }
      else
        counter = TIMER_2;
 
      //write lsb to counter of the timer
      util_get_LSB(divisor, &lsb);

      //write msb to counter of the timer
      util_get_MSB(divisor, &msb);

      if(sys_outb(counter,lsb) == OK)
        return sys_outb(counter,msb);
    }
  }
  return 1;

}

int(timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = TIMER_HOOK_ID;

  //subscribe a notification on every interrupt in the IRQ (interrupt request line) of Timer 0
  //IRQ_REENABLE indicates that the interrupt is going to be enabled by the generic interrupt handler 
  if(sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&TIMER_HOOK_ID) == OK){
    return 0;
  }
  else
    return 1;
}

int(timer_unsubscribe_int)() {
  //unsubscribe a previous subscription of a specific interrupt notification (the one associated with the HOOK_ID)
  if(sys_irqrmpolicy(&TIMER_HOOK_ID) == OK){
    return 0;
  }
  else
    return 1;
}

void(timer_int_handler)() {
    TIMER_COUNTER ++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {

  //setting the RB Command in a uint8_t: D7=1, D6=1 - RB command; D5=1 - don't latch count; D4=0 - latch status;
  uint32_t rb_command = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer)); //REMEMBER: for count=0 we have count_=1 and for status= 1 we have status_=0

  //address of the status
  uint8_t status;

  //checking if the output the rb command regenerates when inputed to the controller is valid

  if (sys_outb(TIMER_CTRL, rb_command) == OK) {
    //Do the input and return the staus byte in the variable status
    if (timer == 0) {
      if (util_sys_inb(TIMER_0, &status) == 0) {
        *st = status;
        return 0;
      }
      return 1;
    }
    else if (timer == 1) {
      if (util_sys_inb(TIMER_1, &status) == 0) {
        *st = status;
        return 0;
      }
      return 1;
    }
    else if (timer == 2) {
      if (util_sys_inb(TIMER_2, &status) == 0) {
        *st = status;
        return 0;
      }
      return 1;
    }
    return 1;
  }
  return 1;
}

int(timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  //uint8_t field_value;
  uint8_t temp;

  //for each type of field, check it's value for the timer "timer" and output it's meaning

  union timer_status_field_val field_value;

  switch (field) {

    case tsf_base:
      field_value.bcd = TIMER_SB_BCD & st;
      break;

    case tsf_mode:
      temp = ((TIMER_SB_MODE & st) >> 1);
      if (temp == 0 || temp == 1 || temp == 4 || temp == 5)
        field_value.count_mode = temp;
      else if (temp == 2 || temp == 6)
        field_value.count_mode = 2;
      else
        field_value.count_mode = 3;
      break;

    case tsf_initial:
      temp = ((TIMER_SB_INIT & st) >> 4);
      if (temp == 1)
        field_value.in_mode = LSB_only;
      else if (temp == 2)
        field_value.in_mode = MSB_only;
      else if (temp == 3)
        field_value.in_mode = MSB_after_LSB;
      else
        field_value.in_mode = INVAL_val;
      break;

    case tsf_all:
      field_value.byte = st;
      break;

    default:
      printf("Invalid Field!\n\n");
      return 1;
  }

  return timer_print_config(timer, field, field_value);
}
