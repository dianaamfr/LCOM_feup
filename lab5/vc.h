#ifndef _LCOM_VC_H
#define _LCOM_VC_H
#include <lcom/lcf.h>

int vbe_get_info_mode(uint16_t mode, vbe_mode_info_t * vmi_p);

int graphics_mode(uint16_t mode);

int (vg_draw_hline)	(	uint16_t x, uint16_t y,uint16_t len, uint32_t color);

int (change_pixel_color)(uint16_t x, uint16_t y, uint32_t color);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

unsigned get_bits_per_pixel();

unsigned get_hres();

unsigned get_vres();

vbe_mode_info_t get_vbe_info_t();

mmap_t * get_mem_map();

uint32_t R(uint16_t col, uint32_t first, uint8_t step);

uint32_t G(uint16_t row, uint32_t first, uint8_t step);

uint32_t B(uint16_t row,uint16_t col, uint32_t first, uint8_t step);

uint32_t red_mask();

uint32_t blue_mask();

uint32_t green_mask();

uint32_t first_green(uint32_t first);

uint32_t first_red(uint32_t first);

uint32_t first_blue(uint32_t first);

int get_xpm(xpm_map_t xmap, uint16_t x, uint16_t y);

void draw_xpm(uint8_t *map, xpm_image_t * img, uint16_t x, uint16_t y);

int video_move_onePixeldisplacement(xpm_map_t xpm,int16_t *x, int16_t *y, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate);

int video_move_consecutiveFramesDisplacement(xpm_map_t xpm, int16_t *x, int16_t *y, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,int16_t speed, uint8_t fr_rate);

int delete_xpm(xpm_map_t xmap, uint16_t x, uint16_t y);

phys_bytes far_pointer(phys_bytes ptr, mmap_t * map);
#endif // _LCOM_VC_H
