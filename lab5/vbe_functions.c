#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdbool.h>
#include <stdint.h>

#include "vbe_macros.h"
#include "vbe_functions.h"

int (get_mode_info)(uint16_t mode){

    struct reg86 reg_86;

    memset(&vbe_mode_info, 0, sizeof(vbe_mode_info_t));

    memset(&reg_86, 0, sizeof(reg_86));

    vbe_mode_info_t *address = lm_alloc(sizeof(vbe_mode_info_t), &map);

    reg_86.intno = VIDEO_CARD;
    reg_86.ah = AH_REG;
    reg_86.al = VBE_INFO_MODE;
    reg_86.cx = mode;
    reg_86.es = PB2BASE(map.phys);
    reg_86.di = PB2OFF(map.phys);

    if (sys_int86(&reg_86) || reg_86.ah){
       
        if (lm_free(&map))
            printf("failed to free memory\n");

        return 1;

    } 

    memcpy(&vbe_mode_info, address, map.size);
    lm_free(&map);
    
    return 0;
}

int (map_vram)(){

    struct minix_mem_range mr;
    unsigned int vram_base = get_phys_address();
    unsigned int vram_size = get_vram_size();

    mr.mr_base = vram_base;
    mr.mr_limit = vram_base + vram_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)){
        
        if (lm_free(&map))
            printf("failed to free memory\n");

        return 1;
    }

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED){

        if (lm_free(&map))
            printf("failed to free memory\n");

        panic("couldn't map video memory\n");
    }

    return 0;
}

phys_bytes (get_phys_address)(void){

    return vbe_mode_info.PhysBasePtr;
}

uint16_t (get_xres)(void){
    
    return vbe_mode_info.XResolution;
}

uint16_t (get_yres)(void){

    return vbe_mode_info.YResolution;
}

uint16_t (get_bits_per_pixel)(void){

    return vbe_mode_info.BitsPerPixel;
}

uint16_t (get_bytes_per_pixel)(void){

    return ((get_bits_per_pixel() + 7) >> 3);
}

unsigned int (get_vram_size)(void){

    return get_xres() * get_yres() * get_bytes_per_pixel();
}

unsigned int (get_RedMaskSize)(void){

    return vbe_mode_info.RedMaskSize;
}

unsigned int (get_GreenMaskSize)(void){

    return vbe_mode_info.GreenMaskSize;
}

unsigned int (get_BlueMaskSize)(void){

    return vbe_mode_info.BlueMaskSize;
}

int (set_graphics_mode)(uint16_t mode){

    struct reg86 reg_86;

    memset(&reg_86, 0, sizeof(reg_86));

    reg_86.intno = VIDEO_CARD;
    reg_86.ah = AH_REG;
    reg_86.al = SET_MODE;
    reg_86.bx = mode | SET_LIN_FR_BUF_MODEL;    

    if (sys_int86(&reg_86) || reg_86.ah)
        return 1;

    return 0;
}

int (vg_set_pixel)(uint16_t x, uint16_t y, uint32_t color){

    if (x < get_xres() || y < get_yres()){

        unsigned int pos = (x + y * get_xres()) * get_bytes_per_pixel();
        memcpy((void*)((unsigned int)video_mem + pos), &color, get_bytes_per_pixel());
        return 0;
    }

    return 1;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){

    uint16_t i;
    
    for (i = 0; i < len; i++)
        if (vg_set_pixel(x + i, y, color))
            return 1;
    
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){

    uint16_t i;
    
    for (i = 0; i < height; i++)
        if (vg_draw_hline(x, y + i, width, color))
            return 1;
    
    return 0;
}

int vg_draw_pixmap_mi(uint8_t *xmap,uint16_t xi,uint16_t ji, xpm_image_t img){
    uint16_t i;
    uint16_t j;
    
    for(j=0;j<img.height;j++){
        for(i=0;i<img.width;i++){
            if(vg_set_pixel(xi+i,ji+j,*(xmap+j*img.width+i)))
                return 1;
        }
    }
    return 0;

}

int (vg_set_pixmap_mi)(uint16_t x, uint16_t y, uint8_t *xmap, xpm_image_t img){

    if (x < get_xres() || y < get_yres()){

        unsigned int pos = (x + y * get_xres()) * get_bytes_per_pixel();
        memcpy((void*)((unsigned int)video_mem + pos), (void *) xmap, img.size);
        return 0;
    }

    return 1;
}
uint32_t (red)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col){
    return (((0xFF & (first >> 16)) + col * step) % (1 << get_RedMaskSize()));
}

uint32_t (green)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col){
    return (((0xFF & (first >> 8)) + row * step) % (1 << get_GreenMaskSize()));
}

uint32_t (blue)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col){
    return (((0xFF & (first)) + (row + col) * step) % (1 << get_BlueMaskSize()));
}

uint32_t (set_red)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col){
    return ((red(first, step, no_rectangles, row, col) & 0xFF) << 16);
}

uint32_t (set_green)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col){
    return ((green(first, step, no_rectangles, row, col) & 0xFF) << 8);
}

uint32_t (set_blue)(uint32_t first, uint32_t step, uint8_t no_rectangles, uint16_t row, uint16_t col){
    return (red(first, step, no_rectangles, row, col) & 0xFF);
}

int (clean_screen)(){
    return vg_draw_rectangle(0, 0, get_xres(), get_yres(), 0);
}
