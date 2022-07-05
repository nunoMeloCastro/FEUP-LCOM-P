#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "kbc.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "utils.h"  
#include "timer.h" 
#include "vbe_macros.h"
#include "vbe_functions.h"
#include "sprite.h"

int (create_sprite)(xpm_map_t xpm, Sprite *sp){

    if (sp == NULL)
        return 1;

    xpm_image_t pic;

    sp -> map = xpm_load(xpm, XPM_INDEXED, &pic);

    sp -> width = pic.width;
    sp -> height = pic.height;

    return 0;
}

int (destroy_sprite)(Sprite *sp){

    if (sp == NULL)
        return 1;

    if (sp -> map)
        free(sp -> map);

    free(sp);

    return 0;
}

int (get_sprite_width)(Sprite *sp){
    return sp -> width;
}

int (get_sprite_height)(Sprite *sp){
    return sp -> height;
}

void (set_sprite_pos)(Sprite *sp, int x, int y){
    sp -> x = x;
    sp -> y = y;
}

void (draw_sprite)(Sprite *sp){

    int i, j;

    for(i = 0; i < sp -> width; i++)

        for(j = 0; j < sp -> height; j++)

            if (sp -> x + i < get_xres() && sp -> y + j < get_yres())
                
                vg_set_pixel(sp -> x + i, sp -> y + j, sp -> map[i + j * sp -> width]);

}
