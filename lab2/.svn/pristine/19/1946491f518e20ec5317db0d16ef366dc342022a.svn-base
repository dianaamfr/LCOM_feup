#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern int32_t COUNTER;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t st;

  if(timer_get_conf(timer,&st) == 0)
    return timer_display_conf(timer, st, field);
  return 1;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {

  if(timer != 0 && timer!= 1 && timer != 2)
    return 1;

  return timer_set_frequency(timer,freq);
}

int(timer_test_int)(uint8_t time) {
  
  if(time < 0)
  return 1;

  int ipc_status;
  message msg;
  uint8_t bit_number;
  int r;
  uint32_t irq_set; /*the generic interrupt handler will send a notification message to the device driver with bit 0 - so we set to 1 the bit 0.*/

//if a interrupt is subscribed

if(timer_subscribe_int(&bit_number) != 0)
    return 1;
    
irq_set = BIT(bit_number);

/*Minix 3 configures Timer 0 to generate interrupts at a default fixed rate of 60 Hz. On every Timer 0 interrupt, Minix 3 should increment a global counter variable -the counter will be incremented by 60 every second. 
Therefore, to get the number of seconds we divide the value in counter for 60 and compare it with the number of seconds we want*/

while((COUNTER/60) < time ) { /* You may want to use a different condition */
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification  - in Lcom we only deal with notifications (kernel message)*/
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */				
            if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
             timer_int_handler();//count plus one in counter
             //every second print elapsed time
	            if(COUNTER % 60 == 0)
	              timer_print_elapsed_time();
            }
            break;
          default:
            break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
       /* no standard messages expected: do nothing */
    }
 }
  if(timer_unsubscribe_int() == 0)
    return 0;
  else
    return 1;


}
