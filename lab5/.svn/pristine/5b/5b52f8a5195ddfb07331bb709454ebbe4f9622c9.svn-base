// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include <vc.h> 
#include <vc_macros.h>
#include <i8042.h>
#include <keyboard.h>

extern uint8_t SCANCODE[2];
extern bool SCANCODE_2B; // true if it is a 2 byte scancode 

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
int(video_test_init)(uint16_t mode, uint8_t delay) {
  
  if (graphics_mode(mode) != 0) {
    /*goes back to default mode - text mode and returns error code*/
    if(vg_exit()!=0) return 1;
    return 1;
  }
  /*waits the time specified in delay and then goes back to default mode - text mode*/
  tickdelay(micros_to_ticks(delay*1000000));
  if(vg_exit()!= 0) return 1;
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  //subscribe the KBC interrupts
  int ipc_status;
  uint8_t bit_number;
  message msg;
  int r;
  uint32_t irq_set; /*bit no returned upon an interrupt subscription*/


  //if a interrupt is subscribed
  if(kbc_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1

  /*Initializes the low memory area, the region up to the 1 MByte physical address, by mapping it on the process' physical memory address - it allows the next step (memory allocation)*/

  /*Set the graphic mode*/
  if (graphics_mode(mode) != 0)return 1;

  if(vg_draw_rectangle(x,y,width,height,color) != 0){
    if(vg_exit()!= 0) return 1;
    return 1;
  }

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

  if(vg_exit()!= 0) return 1;

  if(kbc_unsubscribe_int() != 0)
    return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  //subscribe the KBC interrupts
  int ipc_status;
  uint8_t bit_number;
  message msg;
  int r;
  uint32_t irq_set; /*bit no returned upon an interrupt subscription*/


  //if a interrupt is subscribed
  if(kbc_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1

  /*Initializes the low memory area, the region up to the 1 MByte physical address, by mapping it on the process' physical memory address - it allows the next step (memory allocation)*/

  /*Set the graphic mode*/
  if (graphics_mode(mode) != 0)return 1;

  /*Define the size of the rectangles*/
  uint16_t width = get_hres() / no_rectangles;
  uint16_t height = get_vres() / no_rectangles;



  for(unsigned int row=0; row<no_rectangles; row++){
    if((row*height) >= (get_vres() - get_vres()%no_rectangles)) /*if there is not enough vertical space for the rectangle*/
      break;

    for(unsigned int col = 0; col< no_rectangles; col++){
      if((col*width) >= (get_hres() - get_hres()%no_rectangles))/*if there is not enough horizontal space for the rectangle*/
        break;

      if ((get_vbe_info_t().MemoryModel != DIRECT_COLOR_MODEL1) && (get_vbe_info_t().MemoryModel != DIRECT_COLOR_MODEL2)) {/*if the color model is indexed*/
      /*generate the color index for color model*/
        uint32_t colorIndex = (first + (row * no_rectangles + col) * step) % (1 << get_bits_per_pixel());
        vg_draw_rectangle(col*width,row*height,width,height,colorIndex);
      }
      else{
        uint32_t red = R(col,first,step);
				uint32_t green = G(row,first,step);
				uint32_t blue = B(row,col,first,step);

        /*Join the color components to create the new color*/
        uint32_t newColor = blue | (green<<get_vbe_info_t().GreenFieldPosition) | (red<<(get_vbe_info_t().RedFieldPosition));
        
        vg_draw_rectangle(col*width,row*height,width,height,newColor);
      }
    }
  }

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

  if(vg_exit()!= 0) return 1;

  if(kbc_unsubscribe_int() != 0)
    return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  //subscribe the KBC interrupts
  int ipc_status;
  uint8_t bit_number;
  message msg;
  int r;
  uint32_t irq_set; /*bit no returned upon an interrupt subscription*/


  //if a interrupt is subscribed
  if(kbc_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1

  /*Initializes the low memory area, the region up to the 1 MByte physical address, by mapping it on the process' physical memory address - it allows the next step (memory allocation)*/

  /*Set the graphic mode*/
  if (graphics_mode(MODE_105) != 0)return 1;

  if(get_xpm(xpm,x,y) != 0) return 1;


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

  if(vg_exit()!= 0) return 1;

  if(kbc_unsubscribe_int() != 0)
    return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,int16_t speed, uint8_t fr_rate) {
  int ipc_status;
  uint8_t bit_number;
  message msg;
  int r;
  uint32_t irq_kbc; /*bit no returned upon an interrupt subscription to the kbc*/
  uint32_t irq_timer;/*bit no returned upon an interrupt subscription to the timer*/
  int16_t x= xi, y=yi;
  int tmp;

  /*Set the graphic mode*/
  if (graphics_mode(MODE_105) != 0)return 1;

    //interrupt is subscribed to the keyboard
  if(kbc_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1
  irq_kbc = BIT(bit_number);
  //interrupt is subscribed to the timer 0 - because of the speed parameter
  if(timer_subscribe_int(&bit_number) != 0)
      return 1; // if there is a problem with the subscription return 1
  irq_timer = BIT(bit_number);
  /* display the pixmap provided as an XPM at initial x and initial y - initial position*/
  if(get_xpm(xpm,xi,yi) != 0) return 1;
  
  while(SCANCODE[0] != BC_ESC) { /*enquanto não é feito esc*/ 
    /* Get a request message. */
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
       continue;
  }   if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */
            if (msg.m_notify.interrupts & irq_timer) { /* subscribed interrupt to timer*/
              timer_int_handler();

              if(speed >= 0){/* speed = displacement in pixels between consecutive frames*/
                  tmp=video_move_consecutiveFramesDisplacement(xpm,&x,&y,xi,yi,xf,yf,speed,fr_rate);
              }
              else{/* speed = number of frames required for a displacement of one pixel*/
                tmp = video_move_onePixeldisplacement(xpm,&x,&y,xi,yi,xf,yf,speed,fr_rate);
              }
              if(tmp==1)return 1; /*error thrown*/
              else if(tmp==2) break; /*movement complete*/
            }				
            if (msg.m_notify.interrupts & irq_kbc) { /* subscribed interrupt to kbc*/
              kbc_ih();
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

  if(kbc_unsubscribe_int() != 0)
    return 1;

  if (timer_unsubscribe_int() != 0) {
   return 1;
  }

  if(vg_exit()!= 0) return 1;

  return 0;
}

int(video_test_controller)() {
  mmap_t mem_map; /*mapping of physical memory to virtual memory*/
  vg_vbe_contr_info_t vc;
  vg_vbe_contr_info_t *contr_info = &vc;
  VbeInfoBlock vib;
  VbeInfoBlock * VbeInfoBlockC = &vib;
  reg86_t r;

  /*Must allocate a buffer in low memory with alloc*/
	 lm_alloc(sizeof(VbeInfoBlock),&mem_map);
  
  /*must initialize field VBESignature with value "VBE2" -> So that the information returned is compatible with VBE 2.0*/
  ((char *)mem_map.virt)[0] = 'V';
  ((char *)mem_map.virt)[1] = 'B';
  ((char *)mem_map.virt)[2] = 'E';
  ((char *)mem_map.virt)[3] = '2';

  memset(&r, 0, sizeof(r));
	r.ax = VBE | GET_VBE_CTRL_INFO; /* VBE get controller info - Call VBE function 0x00*/
	/* translate the buffer linear address to a far pointer */
	r.es = PB2BASE(mem_map.phys); /* set a segment base */
	r.di = PB2OFF(mem_map.phys); /* set the offset accordingly */
	r.intno = VC_INT;
  if (sys_int86(&r) != OK) { /* call BIOS */
		return 1;
	}

  /*copy contents from mem_map virtual address to the VbeInfoBlock struct*/
  char *src = (char *)mem_map.virt; 
  char *dest = (char *)VbeInfoBlockC; 
  
  for (int i=0; i<(int)mem_map.size; i++) 
    dest[i] = src[i]; 

  contr_info->VBESignature[0] = VbeInfoBlockC-> VbeSignature[0];
  contr_info->VBESignature[1] = VbeInfoBlockC->VbeSignature[1];
  contr_info->VBESignature[2] = VbeInfoBlockC->VbeSignature[2];
  contr_info->VBESignature[3] = VbeInfoBlockC->VbeSignature[3];

  contr_info->VBEVersion[0] = VbeInfoBlockC->VbeVersion[0];
  contr_info->VBEVersion[1] = VbeInfoBlockC->VbeVersion[1];

  contr_info->TotalMemory = 64*VbeInfoBlockC->TotalMemory; /*because it is in 64kb units and we want 1kb units*/
  
  contr_info->OEMString = (char *)far_pointer(VbeInfoBlockC->OemStringPtr,&mem_map); 

  contr_info->VideoModeList =(uint16_t *)far_pointer(VbeInfoBlockC->VideoModePtr,&mem_map);

  contr_info->OEMVendorNamePtr =(char *)far_pointer(VbeInfoBlockC->OemVendorNamePtr,&mem_map);

  contr_info->OEMProductNamePtr =(char *)far_pointer(VbeInfoBlockC->OemProductNamePtr,&mem_map);

  contr_info->OEMProductRevPtr =(char *)far_pointer(VbeInfoBlockC->OemProductRevPtr,&mem_map);

  /* free the memory allocated it ass soon as possible */
  lm_free(&mem_map);

  vg_display_vbe_contr_info(contr_info);

  return 0;
}
