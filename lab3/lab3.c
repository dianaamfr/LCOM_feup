#include <lcom/lcf.h>

#include <keyboard.h>
#include <i8042.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef LAB3
uint32_t SYS_INB_COUNTER=0;
#endif

extern uint8_t SCANCODE[2];
extern bool SCANCODE_2B; // true if it is a 2 byte scancode 
extern uint32_t COUNTER;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  //subscribe the KBC interrupts
  int ipc_status;
  uint8_t bit_number;
  uint8_t maskMSB= 0x80;
  message msg;
  int r;
  uint32_t irq_set; /*bit no returned upon an interrupt subscription*/


  //if a interrupt is subscribed
  if(kbc_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1


  irq_set = BIT(bit_number);

  while(SCANCODE[0] != BC_ESC) { 
    /* Get a request message. */
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
       continue;
  }   if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */				
            if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */

              //call interrupt handler
              kbc_ih();
                  
              //check for the type of scancode and print it
              if(!SCANCODE_2B && SCANCODE[0] == SCANCODE_2B_FB){//if it is a 2 byte scancode
                if((maskMSB & SCANCODE[1])>>7 == 1)//if it is a break code
                  kbd_print_scancode(false,2,SCANCODE);
                else//if it is a make codes
                  kbd_print_scancode(true,2,SCANCODE);
                }
              else if(!SCANCODE_2B){
                if((maskMSB & SCANCODE[0])>>7 == 1)//if it is a break code
                  kbd_print_scancode(false,1,&SCANCODE[0]);
                else//if it is a make code
                  kbd_print_scancode(true,1,&SCANCODE[0]);
              }
            }
            break;
          default:
            break; /* no other notifications expected: do nothing */	
       }
    }
    else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
 }

  if(kbc_unsubscribe_int() != 0){
    return 1;
  }
  
  #ifdef LAB3
  kbd_print_no_sysinb(SYS_INB_COUNTER);
  #endif

  return 0;
}

int(kbd_test_poll)() {
  uint8_t state,command_byte;
  uint8_t maskMSB= 0x80;
  SCANCODE[0] = 0;
  SCANCODE[1] = 0;


  while(SCANCODE[0] != BC_ESC) {

    kbc_ih(); 

    //check for the type of scancode and print it
    if(!SCANCODE_2B && SCANCODE[0]==SCANCODE_2B_FB){//if it is a 2 byte scancode
      if((maskMSB & SCANCODE[1])>>7 == 1)//if it is a break code
        kbd_print_scancode(false,2,SCANCODE);
      else//if it is a make code
        kbd_print_scancode(true,2,SCANCODE);
      }
    else if(!SCANCODE_2B){
      if((maskMSB & SCANCODE[0])>>7 == 1)//if it is a break code
        kbd_print_scancode(false,1,&SCANCODE[0]);
      else//if it is a make code
        kbd_print_scancode(true,1,&SCANCODE[0]);
    }

    tickdelay(micros_to_ticks(DELAY_US));
  }

  if(kbc_issue_command(&state,KBC_READ_CB) != 0)
    return 1;

  if(kbc_read_command_return(&command_byte) != 0)
    return 1;
  
  command_byte = command_byte| KBC_CB_INT;

  if(kbc_issue_command(&state,KBC_WRITE_CB) != 0)
    return 1;

/*command byte is written as an argument of kbc command 0x60*/
  if(kbc_write_command_args(command_byte)!=0)
    return 1;

  #ifdef LAB3
  kbd_print_no_sysinb(SYS_INB_COUNTER);
  #endif

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  
  //subscribe the KBC interrupts
  int ipc_status;
  uint8_t bit_number;
  uint8_t maskMSB= 0x80;
  message msg;
  int r;
  uint32_t irq_kbd,irq_timer0; /*bit no returned upon an interrupt subscription*/


  //if a interrupt is subscribed
  if(kbc_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1

  irq_kbd = BIT(bit_number);

  if(timer_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1

  irq_timer0 = BIT(bit_number);

  while(((COUNTER/60) < n) && (SCANCODE[0] != BC_ESC)) { 
    /* Get a request message. */
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
       continue;
  }   if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */
            if(msg.m_notify.interrupts & irq_timer0){
              timer_int_handler();//count plus one in counter
            }				
            if (msg.m_notify.interrupts & irq_kbd) { /* subscribed interrupt */

              //call interrupt handler
              kbc_ih();

              //check for the type of scancode and print it
              if(SCANCODE_2B){//if it is a 2 byte scancode
                if((maskMSB & SCANCODE[1])>>7 == 1)//if it is a break code
                  kbd_print_scancode(false,2,SCANCODE);
                else//if it is a make code
                  kbd_print_scancode(true,2,SCANCODE);
                }
              else{
                if((maskMSB & SCANCODE[0])>>7 == 1)//if it is a break code
                  kbd_print_scancode(false,1,&SCANCODE[0]);
                else//if it is a make code
                  kbd_print_scancode(true,1,&SCANCODE[0]);
              }
              COUNTER = 0;
            }
            break;
          default:
            break; /* no other notifications expected: do nothing */	
       }
    }
    else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
 }

  if(kbc_unsubscribe_int() != 0){
    return 1;
  }
  
  #ifdef LAB3
  kbd_print_no_sysinb(SYS_INB_COUNTER);
  #endif

  return 0;

  return 1;
}
