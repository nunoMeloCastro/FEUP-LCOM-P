#ifndef _SPRITE_H_
#define _SPRITE_H_

typedef struct {
  int x,y;             /**< current sprite position */
  int width, height;   /**< sprite dimensions */
  int xspeed, yspeed;  /**< current speeds in the x and y direction */
  uint8_t *map;           /**< the sprite pixmap (use read_xpm()) */
} Sprite;


int create_sprite(xpm_map_t xpm, Sprite *sp);

int destroy_sprite(Sprite *sp);

int get_sprite_width(Sprite *sp);

int get_sprite_height(Sprite *sp);

void set_sprite_pos(Sprite *sp, int x, int y);

void draw_sprite(Sprite *sp);

#endif /* _SPRITE_H_ */
