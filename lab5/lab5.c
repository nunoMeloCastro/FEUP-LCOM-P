// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "kbc.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "utils.h"  
#include "timer.h" 
#include "vbe_macros.h"
#include "vbe_functions.h"
#include "sprite.h"

// Any header files included below this line should have been created by you

extern uint32_t int_counter;

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
  
  if (get_mode_info(mode))
    
    if (vg_exit()) 
      return 1;

  if (map_vram())
    return 1;

  if (set_graphics_mode(mode)){
    
    if (vg_exit()){

      if (lm_free(&map))
        printf("failed to free memory\n");

      return 1;
    }

  }

  tickdelay(micros_to_ticks(delay * 1000000));
  
  if (vg_exit()){

      if (lm_free(&map))
        printf("failed to free memory\n");

      return 1;
    }

  
  if (lm_free(&map))
    printf("failed to free memory\n");

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  
  if (get_mode_info(mode))
    if (vg_exit())
      
      return 1;

  if (map_vram())
    return 1;

  if (set_graphics_mode(mode)){
    
    vg_exit();

    if (lm_free(&map))
      printf("failed to free memory\n");

    return 1;

  }
  
  uint16_t w = (x + width > get_xres()) ? (get_xres() - x) : (width);
  /*if((x + width > get_xres()))
    w=(get_xres() - x);
  else
  {
    w=width;
  }*/
  
  uint16_t h = (y + height > get_yres()) ? (get_yres() - y) : (height);

  if (x < get_xres() && y < get_yres()){
    
    if (vg_draw_rectangle(x, y, w, h, color)){

      vg_exit();

      if (lm_free(&map))
        printf("failed to free memory\n");

        return 1;
    }
  }

  int ipc_status, r;
  message msg;
  uint8_t irq_id = 0;
  uint8_t scancode[2];
  uint8_t code_size = 1;          // 1 if 1 byte size || 2 if 2 byte size 

  if(kbc_subscribe_int(&irq_id)){
    
    vg_exit();

    if (lm_free(&map))
      printf("failed to free memory\n");
  
    return 1;
  }

  int irq_set = BIT(irq_id);

  
  while( code != ESC_BRK_CODE ) { /* You may want to use a different condition */
    /* Get a request message. */
    
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      
      switch (_ENDPOINT_P(msg.m_source)) {
        
        case HARDWARE: /* hardware interrupt notification */
          
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            /* process it */
            
            kbc_int_handler();
            
            if (code == TWO_BYTE_CODE){
              scancode[0] = code;
              code_size = 2;
            }

            else if (code_size == 2){
              scancode[1] = code;
            }

            else {
              scancode[0] = code;
              code_size = 1;
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

  kbc_unsubscribe_int();

  if (vg_exit()){

      if (lm_free(&map))
        printf("failed to free memory\n");

      return 1;
    }

  
  if (lm_free(&map))
    printf("failed to free memory\n");

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  
  if (get_mode_info(mode))
    
    if (vg_exit())
      
      return 1;

  if (map_vram())
    return 1;

  if (set_graphics_mode(mode)){
    
    vg_exit();

    if (lm_free(&map))
      printf("failed to free memory\n");

    return 1;

  }

  uint16_t x_len = get_xres() / no_rectangles;
  uint16_t y_len = get_yres() / no_rectangles;

  uint8_t x_ind, y_ind;
  uint32_t color;

  for (x_ind = 0; x_ind < no_rectangles; x_ind++){
    
    for (y_ind = 0; y_ind < no_rectangles; y_ind++){
        
      if(get_bytes_per_pixel() == 1)
        color = (first + (x_ind * no_rectangles + y_ind) * step) % (1 << get_bits_per_pixel());
      
      else
        color = (set_red(first, step, no_rectangles, x_ind, y_ind) | set_green(first, step, no_rectangles, x_ind, y_ind) | set_blue(first, step, no_rectangles, x_ind, y_ind));

      uint16_t x = y_ind * x_len;
      uint16_t y = x_ind * y_len;

      uint16_t w = (x + x_len > get_xres()) ? (get_xres() - x) : (x_len);
      uint16_t h = (y + y_len > get_yres()) ? (get_yres() - y) : (y_len);

      if (x < get_xres() && y < get_yres()){
        
        if (vg_draw_rectangle(x, y, w, h, color)){

          vg_exit();

          if (lm_free(&map))
            printf("failed to free memory\n");

            return 1;
        }
      }
    }
  }

  int ipc_status, r;
  message msg;
  uint8_t irq_id = 0;
  uint8_t scancode[2];
  uint8_t code_size = 1;          // 1 if 1 byte size || 2 if 2 byte size 

  if(kbc_subscribe_int(&irq_id)){
    
    vg_exit();

    if (lm_free(&map))
      printf("failed to free memory\n");
  
    return 1;
  }

  int irq_set = BIT(irq_id);

  
  while( code != ESC_BRK_CODE ) { /* You may want to use a different condition */
    /* Get a request message. */
    
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      
      switch (_ENDPOINT_P(msg.m_source)) {
        
        case HARDWARE: /* hardware interrupt notification */
          
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            /* process it */
            
            kbc_int_handler();
            
            if (code == TWO_BYTE_CODE){
              scancode[0] = code;
              code_size = 2;
            }

            else if (code_size == 2){
              scancode[1] = code;
            }

            else {
              scancode[0] = code;
              code_size = 1;
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

  kbc_unsubscribe_int();

  if (vg_exit()){

      if (lm_free(&map))
        printf("failed to free memory\n");

      return 1;
    }

  
  if (lm_free(&map))
    printf("failed to free memory\n");

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  
  if (get_mode_info(IND_1024x768))
    
    if (vg_exit())
      return 1;

  if (map_vram())
    return 1;

  if (set_graphics_mode(IND_1024x768)){
    
    vg_exit();

    if (lm_free(&map))
      printf("failed to free memory\n");

    return 1;

  }

  Sprite sp;
  
  create_sprite(xpm, &sp);
  set_sprite_pos(&sp, x, y);
  draw_sprite(&sp);

  int ipc_status, r;
  message msg;
  uint8_t irq_id = 0;
  uint8_t scancode[2];
  uint8_t code_size = 1;          // 1 if 1 byte size || 2 if 2 byte size 

  if(kbc_subscribe_int(&irq_id)){
    
    vg_exit();

    if (lm_free(&map))
      printf("failed to free memory\n");
  
    return 1;
  }

  int irq_set = BIT(irq_id);

  
  while( code != ESC_BRK_CODE ) { /* You may want to use a different condition */
    /* Get a request message. */
    
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      
      switch (_ENDPOINT_P(msg.m_source)) {
        
        case HARDWARE: /* hardware interrupt notification */
          
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            /* process it */
            
            kbc_int_handler();
            
            if (code == TWO_BYTE_CODE){
              scancode[0] = code;
              code_size = 2;
            }

            else if (code_size == 2){
              scancode[1] = code;
            }

            else {
              scancode[0] = code;
              code_size = 1;
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

  kbc_unsubscribe_int();

  if (vg_exit()){

      if (lm_free(&map))
        printf("failed to free memory\n");

      return 1;
    }

  
  if (lm_free(&map))
    printf("failed to free memory\n");

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  
  if (get_mode_info(IND_1024x768))
    
    if (vg_exit())
      return 1;

  if (map_vram())
    return 1;

  if (set_graphics_mode(IND_1024x768)){
    
    vg_exit();

    if (lm_free(&map))
      printf("failed to free memory\n");

    return 1;

  }

  Sprite sp;
  
  create_sprite(xpm, &sp);

  int ipc_status, r;
  message msg;

  uint8_t kb_id = 0;
  uint8_t scancode[2];
  uint8_t code_size = 1;          // 1 if 1 byte size || 2 if 2 byte size 
  int hook_id_k = 0;

  uint8_t timer_id = TIMER0_IRQ;
  int_counter = 0;
  int hook_id_t = 0;

  if(kbc_subscribe_int(&kb_id)){
    
    vg_exit();

    if (lm_free(&map))
      printf("failed to free memory\n");
  
    return 1;
  }

  hook_id_k = hook_id;

  if(timer_subscribe_int(&timer_id))
    return 1;

  hook_id_t = hook_id;

  int kb_irq = BIT(kb_id);
  int timer_irq = BIT(timer_id);

  int16_t v = (speed <= 0 ? 1 : speed);
  int vx = 0;
  int vy = 0;
  
  if (xi != xf)
    vx = (xi < xf ? v : -v);

  if (yi != yf)
    vy = (yi < yf ? v : -v);

  uint32_t freq = sys_hz();
  uint16_t nt = (speed < 0 ? -speed : 1);
  uint32_t ticks_per_frame = freq / (uint32_t)fr_rate;

  uint16_t x = 0;
  uint16_t y = 0;

  set_sprite_pos(&sp, xi, yi);
  clean_screen();
  draw_sprite(&sp);

  while( code != ESC_BRK_CODE) { /* You may want to use a different condition */
    /* Get a request message. */
    
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      
      switch (_ENDPOINT_P(msg.m_source)) {
        
        case HARDWARE: /* hardware interrupt notification */
          
          if (msg.m_notify.interrupts & timer_irq) { /* subscribed interrupt */
            /* process it */
            
            timer_int_handler();
            
            if (int_counter == nt * ticks_per_frame){

              if(vx){
                uint16_t x_clear = (vx > 0 ? x : x + get_sprite_width(&sp) + vx);
                uint16_t y_clear = y;
                uint16_t w = (x + abs(v) > get_xres()) ? (get_xres() - x) : (abs(v));
                uint16_t h = (y + get_sprite_height(&sp) > get_yres()) ? (get_yres() - y) : (get_sprite_height(&sp));
              
                if (x_clear < get_xres() && y_clear < get_yres()){
                  
                  if (vg_draw_rectangle(x_clear, y_clear, w, h , 0)){
                    
                    vg_exit();

                    if (lm_free(&map))
                      printf("failed to free memory\n");

                    return 1;

                  }
                }
              }

              if (vy){
                
                uint16_t x_clear = x;
                uint16_t y_clear = (vy > 0 ? y : y + get_sprite_height(&sp) + vy);
                uint16_t w = (x + get_sprite_width(&sp) > get_xres()) ? (get_xres() - x) : (get_sprite_width(&sp));
                uint16_t h = (y + abs(v) > get_yres()) ? (get_yres() - y) : (abs(v));
              
                if (x_clear < get_xres() && y_clear < get_yres()){
                  
                  if (vg_draw_rectangle(x_clear, y_clear, w, h , 0)){
                    
                    vg_exit();

                    if (lm_free(&map))
                      printf("failed to free memory\n");

                    return 1;

                  }
                }
              }

            vx = (vx > 0 ? MIN(vx, xf - x) : MAX(vx, xf - x));
            vy = (vy > 0 ? MIN(vy, yf - y) : MAX(vy, yf - y));
            x += vx;
            y += vy;
            set_sprite_pos(&sp, x, y);
            draw_sprite(&sp);

            int_counter = 0;

            }
          }

          if (msg.m_notify.interrupts & kb_irq) { /* subscribed interrupt */
            /* process it */
            
            kbc_int_handler();
            
            if (code == TWO_BYTE_CODE){
              scancode[0] = code;
              code_size = 2;
            }

            else if (code_size == 2){
              scancode[1] = code;
            }

            else {
              scancode[0] = code;
              code_size = 1;
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

  hook_id = hook_id_t;
  timer_unsubscribe_int();

  hook_id = hook_id_k;
  kbc_unsubscribe_int();

  if (vg_exit()){

      if (lm_free(&map))
        printf("failed to free memory\n");

      return 1;
    }

  
  if (lm_free(&map))
    printf("failed to free memory\n");

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
