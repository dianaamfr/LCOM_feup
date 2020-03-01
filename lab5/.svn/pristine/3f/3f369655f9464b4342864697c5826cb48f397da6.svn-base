#include <lcom/lcf.h>
#include <vc.h>
#include <vc_macros.h>
#include <keyboard.h>
#include <i8042.h>

static vbe_mode_info_t info;/*structure to be initialized  with the VBE information on the input mode */
static void *video_mem;		/* frame-buffer VM address */
static uint16_t hres;/* Horizontal resolution in pixels */
static uint16_t vres; /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static mmap_t mem_map; /*mapping of physical memory to virtual memory*/

extern uint8_t SCANCODE[2];
extern bool SCANCODE_2B; // true if it is a 2 byte scancode 
extern uint32_t COUNTER;

int vbe_get_info_mode(uint16_t mode, vbe_mode_info_t * vmi_p){
  reg86_t r;

	lm_alloc(sizeof(vbe_mode_info_t),&mem_map);

  memset(&r, 0, sizeof(r));
	r.ax = VBE | GET_VBE_MODE_INFO; /* VBE get mode info */
	/* translate the buffer linear address to a far pointer */
	r.es = PB2BASE(mem_map.phys); /* set a segment base */
	r.di = PB2OFF(mem_map.phys); /* set the offset accordingly */
	r.cx = mode;
	r.intno = VC_INT;
	if (sys_int86(&r) != OK) { /* call BIOS */
		return 1;
	}

  /*copy contents from mem_map virtual address to the vbe info struct*/
  char *src = (char *)mem_map.virt; 
  char *dest = (char *)vmi_p; 
  
  for (int i=0; i<(int)mem_map.size; i++) 
    dest[i] = src[i]; 

  lm_free(&mem_map);

  return 0;
}

int graphics_mode(uint16_t mode){
	/*Allocates a memory block in low memory area with 1MB*/
	int mb= 1024*1024;
	mem_map.phys=0;
	mem_map.size=mb;
	lm_alloc(mb,&mem_map);

	/*Initialize vram_base and vram_size*/
  memset(&info, 0, sizeof(info));/* zero the structure */

	/*for now use the function vbe_get_mode_info to obtain the physical memory address*/
	if(vbe_get_info_mode(mode,&info)!=0)
		return 1;

	unsigned int vram_base; /* VRAM’s physical addresss */
  unsigned int vram_size; /* VRAM’s size, but you can use the frame-buffer size, instead */

	vram_base  = info.PhysBasePtr;
  hres = info.XResolution;
  vres = info.YResolution;
  bits_per_pixel = info.BitsPerPixel;
  vram_size = ((bits_per_pixel+7)/8)*hres*vres; //using frame buffer size

	/* Allow memory mapping */
	struct minix_mem_range mr; /* physical memory range */
	int r;
  mr.mr_base = (phys_bytes)vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if(video_mem == MAP_FAILED)
    panic("couldn’t map video memory");

  //Initialize Video card with Graphic Mode
  reg86_t r86;
  memset(&r86, 0, sizeof(r86));	/* zero the structure */

	r86.intno = VC_INT; /* BIOS video services */
  r86.ax = VBE | SET_VBE_MODE;
  r86.bx = SET_LINEAR_MODE | mode;
  
  if( sys_int86(&r86) != OK ) { 
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }
	lm_free(&mem_map);
	return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){

  for(int i = 0; i < len ; i++){
    if(x+i >= hres) return 0;//deal with illegal horizontal dimensions
    if(change_pixel_color(x+i,y,color) != 0) return 1;
  }
  return 0; 
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for(int i = 0; i < height; i++){
    if(y+i >= vres) return 0; //deal with illegal vertical dimensions
    if(vg_draw_hline(x,y+i,width,color) != 0) return 1;
  }
  return 0;
}

int (change_pixel_color)(uint16_t x, uint16_t y, uint32_t color){ 

  if(x >= hres || y>= vres) return 0; //deal with illegal dimensions - for xpm

  uint8_t * pix_ad = (uint8_t*) video_mem + x *((bits_per_pixel + 7)/8) + (y*hres*((bits_per_pixel + 7)/8)); /*address of the pixel to change color*/

  if ((get_vbe_info_t().MemoryModel != DIRECT_COLOR_MODEL1) && (get_vbe_info_t().MemoryModel != DIRECT_COLOR_MODEL2)){ /*if the color model is indexed*/
    color = color & 0xFFFF; //keep only the 2 less significant bytes
  }
  else{/*if the color model is direct*/
    color = color & 0xFFFFFF; //keep only the 3 less significant bytes
    }

  uint8_t temp;

  for(unsigned int i = 0; i < (bits_per_pixel + 7)/8; i++){ /*changes the number of bytes per pixel*/
    temp = color & 0xFF; /*last byte of the color*/
    *(pix_ad + i) = temp; //next pixel
    color = color >> 8; /*next byte of the color*/
  }

  return 0;
}

unsigned get_bits_per_pixel(){
  return bits_per_pixel;
}

unsigned get_hres(){
  return hres;
}

unsigned get_vres(){
  return vres;
}

 mmap_t * get_mem_map(){
  return &mem_map;
}

vbe_mode_info_t get_vbe_info_t(){
  return info;
}

uint32_t red_mask() {
  return ((1 << info.RedMaskSize)-1);
}

uint32_t blue_mask() {
  return ((1 << info.BlueMaskSize)-1);
}

/*Create */
uint32_t green_mask() {
  return ((1 << info.GreenMaskSize)-1);
}

uint32_t R(uint16_t col, uint32_t first, uint8_t step) {
  return (first_red(first) + col * step) % (1 << info.RedMaskSize);
}

uint32_t G(uint16_t row, uint32_t first, uint8_t step) {
  return (first_green(first) + row * step) % (1 << info.GreenMaskSize);
}

uint32_t B(uint16_t row,uint16_t col, uint32_t first,uint8_t step) {
  return (first_blue(first) + (row + col)*step) % (1 << info.BlueMaskSize);
}

uint32_t first_red(uint32_t first){
  return (first >> info.RedFieldPosition) & red_mask();
}

uint32_t first_blue(uint32_t first){
  return (first >> info.BlueFieldPosition) & blue_mask();
}

uint32_t first_green(uint32_t first){
  return (first >> info.GreenFieldPosition) & green_mask();
}

int get_xpm(xpm_map_t xmap, uint16_t x, uint16_t y){
  uint8_t *map;
  xpm_image_t img;

  // get the pixmap from the XPM
  map = xpm_load(xmap, XPM_INDEXED, &img);
  // copy it to graphics memory

  if(map == NULL) return 1;

  draw_xpm(map,&img,x,y);

  return 0;
}

void draw_xpm(uint8_t *map, xpm_image_t * img, uint16_t x, uint16_t y){
  unsigned int idx=0;
  for(unsigned int i=0; i<img->height; i++){
    for(unsigned int j= 0; j<img->width; j++){
      uint32_t color = map[idx];
      change_pixel_color(x+j,y+i,color);
      idx++;
    }
  }
}

int delete_xpm(xpm_map_t xmap, uint16_t x, uint16_t y){
  uint8_t *map;
  xpm_image_t img;

  map = xpm_load(xmap, XPM_INDEXED, &img);

  if(map == NULL) return 1;

  /*change pixel color to the color of the screen -> delete image*/
  for(unsigned int i=0; i<img.height; i++){
    for(unsigned int j= 0; j<img.width; j++){
      change_pixel_color(x+j,y+i,0);
    }
  }

  return 0;
}

int video_move_consecutiveFramesDisplacement(xpm_map_t xpm, int16_t *x, int16_t *y, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,int16_t speed, uint8_t fr_rate){
  speed = abs(speed);
  /* speed = displacement in pixels between consecutive frames*/
  if(COUNTER%(60/fr_rate) == 0 && ((*x<xf && yi==yf && xi<=xf)||( *y<yf && xi == xf && yi<=yf)||(*x>xf && yi==yf && xi>xf)||( *y>yf && xi == xf && yi>yf))){ /*if there is a frame to show and there is at least one movement to do in one of the axis (before the final position)*/
    if(delete_xpm(xpm,*x,*y)!=0) return 1;//clean the last frame
        
    if(xi==xf){
      if(yf>=yi)
        *y= *y + speed;
      else{
        *y= *y - speed;
      }
    } /*if the movement is in y axis move speed pixels in that axis- displacement in pixels between consecutive frames*/
    else if(yi==yf){
      if(xf>=xi)
        *x= *x + speed;
      else{
        *x= *x - speed;
      }
    }/*if the movement is in x axis move speed pixels in that axis- displacement in pixels between consecutive frames*/
        
    if((*x>=xf && yi==yf && xi<=xf) ||(*y>=yf && xi==xf && yi<=yf)||(*y<=yf && xi==xf && yi>yf) || (*x<=xf && yi==yf && xi>xf)){/* if we are in the final position*/
      if(get_xpm(xpm,xf,yf) != 0) return 1;
      return 2;
    }
    else
    {
      if(get_xpm(xpm,*x,*y)!= 0) return 1;
    }
  }				
  return 0;
}

int video_move_onePixeldisplacement(xpm_map_t xpm,int16_t *x, int16_t *y, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate){
  speed = abs(speed);
  
    /* speed = number of frames required for a displacement of one pixel*/
  if(COUNTER%speed == 0){ /*If the time is a multiple of speed then it's time to move to the next frame*/
    if(COUNTER%(60/fr_rate) == 0 && ((*x<=xf && yi==yf && xi<=xf)||( *y<=yf && xi == xf && yi<=yf)||(*x>=xf && yi==yf && xi>xf)||( *y>=yf && xi == xf && yi>yf))){ /*if there is a frame to show and there is a movement to do in the non static axis - the final postition was not reached yet*/
      if(delete_xpm(xpm,*x,*y)!=0) return 1;//clean the last frame
        
      if(xi==xf){
        if(yi<=yf){
          *y= *y+1;
        }
        else
        {
          *y=*y-1;
        }
      } /*if the movement is in the y axis, move one pixel in that axis- displacement of a pixel*/
      else if(yi==yf){
        if(xi<=xf){
          *x= *x+1;
        }
        else
        {
          *x=*x-1;
        }
      }
       /*if the movement is in the x axis, move one pixel in that axis - displacement of a pixel*/
        
      if((*x>=xf && yi==yf && xi<=xf) ||( *y>=yf && xi==xf && yi<=yf)||(*y<=yf && xi==xf && yi>yf)||(*x<=xf && yi==yf && xi>xf)){/* if we are in the final position*/
        if(get_xpm(xpm,xf,yf) != 0) return 1;
        return 2;
      }
      else
      {
        if(get_xpm(xpm,*x,*y)!= 0) return 1;
      }
    }
  }
  return 0;
}

phys_bytes far_pointer(phys_bytes ptr, mmap_t * map){
  phys_bytes msb = (ptr & 0xFFFF0000)>>12;
  phys_bytes lsb = (ptr & 0xFFFF);

  return (phys_bytes)map->virt + (msb|lsb) - (phys_bytes)map->phys;
}
