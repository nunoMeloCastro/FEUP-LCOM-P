#ifndef _VBE_MACROS_H_
#define _VBE_MACROS_H_

#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdbool.h>
#include <stdint.h>

static vbe_mode_info_t vbe_mode_info;
static mmap_t map;
static void *video_mem;

int (get_mode_info)(uint16_t mode);

int (map_vram)(void);

phys_bytes (get_phys_address)(void);

uint16_t (get_xres)(void);

uint16_t (get_yres)(void);

uint16_t (get_bits_per_pixel)(void);

uint16_t (get_bytes_per_pixel)(void);

unsigned int (get_vram_size)(void);

int (set_graphics_mode)(uint16_t mode);

int (vg_set_pixel)(uint16_t x, uint16_t y, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

uint32_t (red)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col);

uint32_t (gree)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col);

uint32_t (blue)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col);

uint32_t (set_red)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col);

uint32_t (set_green)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col);

uint32_t (set_blue)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col);

int (clean_screen)();

#endif /* _VBE_MACROS_H_ */
