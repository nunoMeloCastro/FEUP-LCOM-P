#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

uint32_t int_counter;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  uint8_t conf = 0;
  uint8_t set_timer = 0;
  uint16_t rate = (uint16_t)(TIMER_FREQ / freq);
  uint8_t lsb = 0;
  uint8_t msb = 0;

  if(timer_get_conf(timer, &conf))
    return 1;

  if(util_get_LSB(rate, &lsb))
    return 1;
  
  if(util_get_MSB(rate, &msb))
    return 1;

  
  switch (timer){

    case 0:
      conf = ((conf & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) | TIMER_SEL0 | TIMER_LSB_MSB);
      set_timer = TIMER_0;
      break;
    
    case 1:
      conf = ((conf & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) | TIMER_SEL1 | TIMER_LSB_MSB);
      set_timer = TIMER_1;
      break;

    case 2:
      conf = ((conf & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) | TIMER_SEL2 | TIMER_LSB_MSB);
      set_timer = TIMER_2;
      break;
    
    default:
      return 1;
  }
  
  if(sys_outb(TIMER_CTRL, conf))
    return 1;

  if (sys_outb(set_timer, lsb))
    return 1;
  
  if (sys_outb(set_timer, msb))
    return 1;

  return 0;
}

int hook_id = TIMER0_IRQ;

int (timer_subscribe_int)(uint8_t *bit_no) {

  *bit_no = hook_id;
  
  if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id))
    return 1;

  return 0;
}

int (timer_unsubscribe_int)() {
  
  if (sys_irqrmpolicy(&hook_id))
    return 1;

  return 0;
}

void (timer_int_handler)() {
  int_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  
  uint8_t cmd = 0;
  uint8_t status = 0;
  cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if(sys_outb(TIMER_CTRL, cmd))
    return 1;

  switch (timer){

    case 0:
      if(util_sys_inb(TIMER_0, &status))
        return 1;
      break;
    
    case 1:
      if(util_sys_inb(TIMER_1, &status))
        return 1;
      break;

    case 2:
      if(util_sys_inb(TIMER_2, &status))
        return 1;
      break;

    default:
      break;
  }

  *st = status;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  
  union timer_status_field_val val;
  uint8_t x;
  
  switch (field){

    case tsf_all:

      val.byte = st;
      break;

    case tsf_initial:
      
      switch ((x = st & (BIT(5) | BIT(4))) >> 4){
        
        case 0:
          val.in_mode = INVAL_val;
          break;
        
        case 1:
          val.in_mode = LSB_only;
          break;

        case 2:
          val.in_mode = MSB_only;
          break;

        case 3:
          val.in_mode = MSB_after_LSB;
          break;
        
        default:
          return 1;
      }
      break;

    case tsf_mode:
      
      switch ((x = st & (BIT(3) | BIT(2) | BIT(1))) >> 1){

        case 0:
          val.count_mode = 0;
          break;

        case 1:
          val.count_mode = 1;
          break;

        case 2:
          val.count_mode = 2;
          break;

        case 3:
          val.count_mode = 3;
          break;

        case 4:
          val.count_mode = 4;
          break;

        case 5:
          val.count_mode = 5;
          break;

        case 6:
          val.count_mode = 2;
          break;

        case 7:
          val.count_mode = 3;
          break;

        default:
          return 1;
      }
      break;

    case tsf_base:
      
      val.bcd = st & BIT(0);
      break;  
    
    default:
      return 1;
  }

  timer_print_config(timer, field, val);
  return 0;
}
