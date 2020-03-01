#include <lcom/lcf.h>
#include <mouse.h>
#include <keyboard.h>
#include <i8042.h>
#include <utils.h>
#include <stdint.h>

int MOUSE_HOOK_ID = MOUSE_IRQ;
uint8_t PACKET[3]={0,0,0};
int PACKET_BYTE_COUNTER=0;

int (mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = MOUSE_HOOK_ID;

  //subscribe a notification on every interrupt in the IRQ12 (interrupt request line) of the Mouse
  if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &MOUSE_HOOK_ID) != OK){
    return 1;
  }
  return 0;
}

int (mouse_unsubscribe_int()) {
  //unsubscribe a previous subscription of a specific interrupt notification (the one associated with the HOOK_ID)
  if(sys_irqrmpolicy(&MOUSE_HOOK_ID) != OK){
    return 1;
  }
  return 0;
}

void (mouse_ih)(void) {
  uint8_t packet_byte;

  if(util_sys_inb(OUT_BUF,&packet_byte) != 0) return;
  PACKET[PACKET_BYTE_COUNTER % 3] = packet_byte;
  PACKET_BYTE_COUNTER++;
}

void build_packet_struct(struct packet *p, uint8_t *packet){

  (*p).bytes[0] = packet[0];
  (*p).bytes[1] = packet[1];
  (*p).bytes[2] = packet[2];

  (*p).lb = (packet[0] & B1_LEFT);
  (*p).rb = ((packet[0] & B1_RIGHT));
  (*p).mb = ((packet[0] & B1_MID));

  if(((packet[0] & B1_XSIGN)) != 0) //x is negative
    (*p).delta_x = -((packet[1] ^= 0xFF) + 1);
  else
    (*p).delta_x = packet[1];
  
  if(((packet[0] & B1_YSIGN)) != 0) //x is negative
    (*p).delta_y = -((packet[2] ^= 0xFF) + 1);
  else
    (*p).delta_y = packet[2];

  (*p).x_ov = ((packet[0] & B1_XOVFL));
  (*p).y_ov = ((packet[0] & B1_YOVFL));
}

int mouse_disable() {
  if (sys_irqdisable(&MOUSE_HOOK_ID) != 0) 
    return 1;
  return 0;
}

int mouse_enable() {
    if (sys_irqenable(&MOUSE_HOOK_ID) != 0)
        return 1;
    return 0;
}

int mouse_enable_data_rep() {
  if(mouse_disable() != 0)
    return 1;

  if(mouse_write_command(MC_EN_DATA_REP) != 0) 
    return 1;
    
  if(mouse_enable() != 0)
    return 1;

  return 0;
}

int mouse_disable_data_rep() {
    if(mouse_disable() != 0)
      return 1;

    if(mouse_write_command(MC_DIS_DATA_REP) != 0)
        return 1;

    if(mouse_enable() != 0)
      return 1;
    return 0;
}

int mouse_write_command(uint8_t cmd){
  int fail = 0;
  int failLimit = 5;
  uint8_t state;
  uint8_t ackB;

    while (fail < failLimit) {
      if(kbc_issue_command(&state,KBC_WRITE_MOUSE) != 0){
        tickdelay(micros_to_ticks(DELAY_US));
        fail++;
      }
      else{
        //mouse commands are written as arguments to de write mouse kb command
        if(kbc_write_command_argument(cmd) != 0)
          return 1;

        

        if(mouse_check_ack(&ackB)!=0)
          return 1;

        if(ackB == MC_ACK) return 0;
        else if(ackB == MC_NACK) fail++;
        else return 1;
        
      }
    };
    return 1; // after 5 error quits trying
}

int mouse_check_ack(uint8_t *ackB){
  if(kbc_read_command_return(ackB) != 0)
    return 1;
  return 0;
}

int actual_x_len = 0, actual_y_len = 0;
bool drawing_first_line = false; // true if we are drawing the first line
bool drawing_second_line = false; // true if we are drawing the second line
bool first_drawing_complete=false; // true if the first line is complete
state_t state = INIT;

void mouse_check_event(struct packet *pp, ev_type_t *evt)
{
  //left button pressed and no other
  bool left_pressed = (pp->lb == 1 && pp->rb != 1 && pp->mb != 1 );
  //right button pressed and no other
  bool right_pressed = (pp->rb && !pp->lb && !pp->mb);
  //none of the buttons are pressed
  bool none_pressed = (!pp->rb && !pp->lb && !pp->mb);
  
  //get correspondent event
  if (left_pressed){ // if the left button is pressed
      *evt = LEFT_DOWN;
      if(!drawing_first_line) // if the first line is already drawn
          drawing_first_line = true;
  }                  
  else if(none_pressed && drawing_first_line) { // if no button is pressed and the drawing_first_line is true -> the first line is complete
      *evt = LEFT_UP;
      drawing_first_line = false; // not drawing the first line anymore
  }          
  else if(right_pressed) {
      *evt = RIGHT_DOWN; // if the right button is pressed
      if(!drawing_second_line && first_drawing_complete) //if we are not drawing the second line yet and the first line was already drawn
          drawing_second_line = true; //start drawing the second line
  }
  else if(none_pressed && drawing_second_line) { // if no button is pressed and the drawing_second_line is true -> the second line is complete
      *evt = RIGHT_UP;
      drawing_second_line = false; //not drawing the second line anymore
  } 
  else if (!none_pressed) //if no button is pressed(right and left)
      *evt = INVALID;    //the event is not the final one
}

int mouse_handle_event(struct packet *pp, ev_type_t evt,  uint8_t x_len, uint8_t tolerance){
   switch(state) {
    case INIT:
      actual_x_len = 0;
      actual_y_len = 0;
      if( evt == LEFT_DOWN)   // Inical state, detects left mouse button, starts drawing state
        state = DRAWUP;
      break;
    case DRAWUP:
      if(evt == LEFT_DOWN){  // if LMB is still pressed
        if (pp->delta_x >= (-1)*(int)tolerance && pp->delta_y >= (-1)*(int)tolerance){  // checks if path is correct
          actual_x_len += pp->delta_x;
          actual_y_len += pp->delta_y; 
        }
        else{  // if path isn't correct, restarts path
          actual_x_len = 0;
          actual_y_len = 0;
        }
      }
      else if( evt == LEFT_UP ){  // if LMB is released, checks if 1º half is finished
          if(actual_x_len >= x_len && actual_y_len > actual_x_len &&  abs(pp->delta_x) <= tolerance && abs(pp->delta_y) <= tolerance){   // if first half is finished, state is at vertex
              state = VERTEX;
              first_drawing_complete=true;
          }
      }
      else // if Any other button is pressed, returns to incial state
          state = INIT;
      break;
    case VERTEX:
        actual_x_len = 0;
        actual_y_len = 0;
        if( evt == RIGHT_DOWN )  // At vertex, if RMB is pressed, starts next sequence
            state = DRAWDOWN;
        else if (evt == LEFT_DOWN) {  // If LMB is pressed, resets to first sequence and resets x and y values
            state = DRAWUP;
            actual_x_len = 0;
            actual_y_len = 0;
        }
        else if(evt == INVALID)  // if Any other button is pressed, returns to incial state
            state = INIT;
        break; 
    case DRAWDOWN:
        if( evt == RIGHT_DOWN ){  // if RMB is still pressed
          if (pp->delta_x >= (-1)*(int)tolerance && pp->delta_y <= tolerance){  // checks if path is correct
            actual_x_len += pp->delta_x;
            actual_y_len += pp->delta_y;  
          }
          else {  // if path isn't correct, restarts path
            actual_x_len = 0;
            actual_y_len = 0;
            drawing_second_line = false;
          }
        }
        else if(evt == RIGHT_UP){ // if LRB is released, checks if 2º half is finished
            if(actual_x_len >= x_len && abs(actual_y_len) > actual_x_len &&  abs(pp->delta_x) <= tolerance && abs(pp->delta_y) <= tolerance)
                return 0;  // Figure is drawn correctly
            }
        else if (evt == LEFT_DOWN){  // If LMB is pressed, resets to first sequence and resets x and y values
            state = DRAWUP;
            actual_x_len = 0;
            actual_y_len = 0;
        }
        else  // if Any other button is pressed, returns to incial state
            state = INIT;
        break;  
    default:
        break;    
    }

    return 1;
}
