// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include <keyboard.h>
#include <mouse.h>
#include <i8042.h>

extern int MOUSE_HOOK_ID;
extern uint8_t PACKET;
extern int PACKET_BYTE_COUNTER;
extern uint32_t TIMER_COUNTER;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (mouse_test_packet)(uint32_t cnt) {
    
  int ipc_status;
  uint8_t bit_number;
  message msg;
  int r;
  uint32_t irq_set;
  struct packet p;

  //if an interrupt is subscribed
  if(mouse_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1
  irq_set = BIT(bit_number);

  // //enable stream mode and data reporting by mouse in stream mode
  if(mouse_enable_data_rep() != 0)
    return 1;

  while(cnt > 0) { 
    /* Get a request message. */
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
       continue;
  }   if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */				
            if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */

              //call interrupt handler
              mouse_ih();
                  
              //check which byte we are dealing with
              if(PACKET_BYTE_COUNTER % 3 == 0){
                build_packet_struct(&p,&PACKET);
                cnt --;
                mouse_print_packet(&p);
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

  if (mouse_disable_data_rep() != 0)
    return 1;
  if(mouse_unsubscribe_int() != 0){
    return 1;
  }

  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  uint8_t state;
  struct packet p;

  // //enable remote mode
  if(mouse_write_command(MC_SET_REMOTE_MODE) != 0 != 0)
    return 1;

  while(cnt > 0) { 
    
    if (mouse_write_command(MC_READ_DATA) != 0)
        return 1;
        
    for(unsigned int i = 0; i< 3; i++){
      mouse_ih();
      tickdelay(micros_to_ticks(DELAY_US));
    }
    build_packet_struct(&p,&PACKET);
    cnt --;
    mouse_print_packet(&p);
    
    tickdelay(micros_to_ticks(period*1000)); // convert milliseconds to seconds
  }
  
  //sets streaming mode
  if (mouse_write_command(MC_SET_STREAM_MODE) != 0)
    return 1;

  //disables data reporting
  if(mouse_write_command(MC_DIS_DATA_REP) != 0) 
    return 1;
  
  //resets the KBC command byte to Minix's default value
  if(kbc_issue_command(&state,KBC_WRITE_CB) != 0)
    return 1;
  if(kbc_write_command(minix_get_dflt_kbc_cmd_byte())!=0)
    return 1;
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  uint8_t bit_number;
  message msg;
  int r;
  uint32_t irq_mouse,irq_timer0;
  struct packet p; 

  //if an interrupt is subscribed to the mouse
  if(mouse_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1
  irq_mouse = BIT(bit_number);

  //if an interrupt is subscribed to the timer
  if(timer_subscribe_int(&bit_number) != 0)
    return 1; // if there is a problem with the subscription return 1
  irq_timer0 = BIT(bit_number);

  // enable stream mode and data reporting by mouse in stream mode
  if(mouse_enable_data_rep() != 0)
    return 1;

  while((TIMER_COUNTER/60) < idle_time) { 
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){ 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if(msg.m_notify.interrupts & irq_timer0){/* subscribed timer interrupt */
              timer_int_handler();//count plus one in counter
          }	
          if (msg.m_notify.interrupts & irq_mouse) { /* subscribed mouse interrupt */
            //call interrupt handler
            mouse_ih();
                  
            //check which byte we are dealing with
            if(PACKET_BYTE_COUNTER % 3 == 0){
              build_packet_struct(&p,&PACKET);
              mouse_print_packet(&p);
              TIMER_COUNTER = 0;
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

  if (mouse_disable_data_rep() != 0)
    return 1;
  if(mouse_unsubscribe_int() != 0){
    return 1;
  }
  if(timer_unsubscribe_int() != 0){
    return 1;
  }

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int ipc_status;
  uint8_t bit_number;
  message msg;
  int r;
  uint32_t irq_set;
  struct packet p;
  ev_type_t event;
  bool end = false;

  //if an interrupt is subscribed
  if(mouse_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1
  irq_set = BIT(bit_number);

  // //enable stream mode and data reporting by mouse in stream mode
  if(mouse_enable_data_rep() != 0)
    return 1;

  while(!end) { 
    /* Get a request message. */
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
       continue;
  }   if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */				
            if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */

              //call interrupt handler
              mouse_ih();
                  
              //check which byte we are dealing with
              if(PACKET_BYTE_COUNTER % 3 == 0){
                build_packet_struct(&p,&PACKET);
                mouse_print_packet(&p);

              mouse_check_event(&p, &event);
              end = !mouse_handle_event( &p,event, x_len, tolerance);
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

  if (mouse_disable_data_rep() != 0)
    return 1;
  if(mouse_unsubscribe_int() != 0){
    return 1;
  }

  return 0;
}
