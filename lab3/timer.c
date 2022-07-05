#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
    uint8_t conf;
    uint8_t lsb = 0, msb = 0;
    timer_get_conf(timer,&conf);

  uint32_t valor = (TIMER_FREQ / freq); //porque

  util_get_LSB(valor,&lsb);
  util_get_MSB(valor,&msb);

 if (timer == 0) {
			conf = ((conf & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) | TIMER_SEL0 | TIMER_LSB_MSB); 
      
			if (sys_outb(TIMER_CTRL, conf) != 0) {
        return 1;
        }
			if (sys_outb(TIMER_0, lsb) != 0) {
        return 1;
        }
			if (sys_outb(TIMER_0, msb) != 0) {
        return 1;
        }
      return 0;
  }


  if (timer == 1) {
			conf = ((conf & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) | TIMER_SEL1 | TIMER_LSB_MSB); 
      
			if (sys_outb(TIMER_CTRL, conf) != 0) {
        return 1;
        }
			if (sys_outb(TIMER_1, lsb) != 0) {
        return 1;
        }
			if (sys_outb(TIMER_1, msb) != 0) {
        return 1;
        }
      return 0;
  }

  if (timer == 2) {
			conf = ((conf & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) | TIMER_SEL2 | TIMER_LSB_MSB); 
      
			if (sys_outb(TIMER_CTRL, conf) != 0) {
        return 1;
        }
			if (sys_outb(TIMER_2, lsb) != 0) {
        return 1;
        }
			if (sys_outb(TIMER_2, msb) != 0) {
        return 1;
        }
      return 0;
  }


  return 1;
}

uint32_t t_counter = 0;
int hook_id;

int (timer_subscribe_int)(uint8_t *bit_no) {
  
 if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE,&hook_id) != 0)
		return 1;

  bit_no=(uint8_t *)&hook_id;  
 	return 0;
}

int (timer_unsubscribe_int)() {
	if (sys_irqrmpolicy(&hook_id) != 0 ) {
 		return 1;
	}
	return 0;
}

void (timer_int_handler)() {
 t_counter++;
}


int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(sys_outb(TIMER_CTRL,TIMER_RB_CMD|TIMER_RB_COUNT_|TIMER_RB_SEL(timer))!=0){ //ESCREVE READ BACK COMMAND NO REGISTO DE CONTROLO (PORT 43) 
    return 2;

  }
   if(timer==0){
      if(util_sys_inb(TIMER_0,st)!=0)
        return 2;
    }
    else if(timer==1){
        if(util_sys_inb(TIMER_1,st)!=0)
          return 2;
      }

      else if(timer==2){
          if(util_sys_inb(TIMER_2,st)!=0)
            return 2;
          
        }
      return 0;
    }
 



int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  
  union timer_status_field_val val;
  if(field==tsf_all){
    val.byte=st;
    timer_print_config(timer,field,val);
  }

  if(field==tsf_initial){
    uint8_t x=(st & (BIT(5)|BIT(4)))>>4;      //76543210    (st<<2)= 54321000    >>6 000000054    
   
    if(x==1){
      val.in_mode=LSB_only;
      timer_print_config(timer,field,val);

    }

    else if(x==2){
      val.in_mode=MSB_only;
      timer_print_config(timer,field,val);
    }

    else if(x==3){
      val.in_mode=MSB_after_LSB;
      timer_print_config(timer,field,val);
    }
    else{
      val.in_mode=INVAL_val;
      timer_print_config(timer,field,val);
    }  
    return 0;
  }


  if(field==tsf_mode){
    uint8_t x=(st & (BIT(3)|BIT(2)|BIT(1)))>>1;
    switch(x){
      case 0:
        val.count_mode=0;
        timer_print_config(timer,field,val);
        break;

      case 1:
        val.count_mode=1;
        timer_print_config(timer,field,val);
        break;

      case 2:
        val.count_mode=2;
        timer_print_config(timer,field,val);
        break;

      case 3:
        val.count_mode=3;
        timer_print_config(timer,field,val);
        break;

      case 4:
        val.count_mode=1;
        timer_print_config(timer,field,val);
        break;

      case 5:
        val.count_mode=1;
        timer_print_config(timer,field,val);
        break;

      case 6:
        val.count_mode=2;
        timer_print_config(timer,field,val);
        break;

      case 7:
        val.count_mode=3;
        timer_print_config(timer,field,val);
        break;



      default:
        return 1;
        
    }
    return 0;
    
  }
  if(tsf_base==field){
    uint8_t x=(st & BIT(0));
    switch(x){
      case 0:
        val.bcd=FALSE;
        timer_print_config(timer,field,val);
        break;

      case 1:
        val.bcd=TRUE;
        timer_print_config(timer,field,val);
        break;
      
      default:
        return 1;
        break;
  }

  return 0;
}
  return 1;
}
