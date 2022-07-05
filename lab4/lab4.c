// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab4.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "kbc.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "utils.h"  
#include "timer.h" 
#include "mouse.h" 
#include "gesture_machine.h"

extern uint8_t code;
extern uint32_t int_counter;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  
  int ipc_status, r;
  message msg;
  uint8_t irq_id = 0;
  struct packet pkt;
  uint8_t counter = 1;
  uint8_t scancode[3];

  if (mouse_subscribe_int(&irq_id))
    return 1;

  if (sys_irqdisable(&hook_id)) //temos de fazer disable para limpar o hook_id?
    return 1;

  if (mouse_set_data_report(1)) //nao percebo esta funçao- no enunciado so refer a uma fuçao mouse_enable_data_reporting()
    return 1;

  if (sys_irqenable(&hook_id))  //isto é para voltar a escrever?
    return 1;

  int irq_set = BIT(irq_id);
  
  while( cnt != 0 ) { /* You may want to use a different condition */
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
            
            mouse_ih();
            
            if ((counter == 1) && (code & BIT1_IDENTFIER)){
              scancode[0] = code;
              counter++;
            }
            
            else if (counter == 2){
              scancode[1] = code;
              counter++;
            }

            else if (counter == 3){
              scancode[2] = code;
              counter++;
            }
            
            if (counter > 3){
              mouse_packet_builder(&pkt, scancode); //esta funçao foi criada por nos?
              mouse_print_packet(&pkt);
              counter = 1;
              cnt--;
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

  if (sys_irqdisable(&hook_id))
    return 1;

  if (mouse_set_data_report(0))
    return 1;

  if (sys_irqenable(&hook_id))
    return 1;

  if(kbc_unsubscribe_int()) 
    return 1;

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    
  int ipc_status, r;
  message msg;
  uint8_t idle_aux = idle_time;
  
  uint8_t timer_id = TIMER0_IRQ;
  int_counter = 0;
  int hook_id_t = 0;
  
  uint8_t mouse_id = MOUSE_IRQ;
  struct packet pkt;
  uint8_t counter = 1;
  uint8_t scancode[3];
  int hook_id_m = 0;

  if (mouse_subscribe_int(&mouse_id))
    return 1;

  if (sys_irqdisable(&hook_id))
    return 1;

  if (mouse_set_data_report(1))
    return 1;

  if (sys_irqenable(&hook_id))
    return 1;

  hook_id_m = hook_id;

  if(timer_subscribe_int(&timer_id))
    return 1;
  hook_id_t = hook_id;

  int mouse_set = BIT(mouse_id);
  int timer_set = BIT(timer_id);
  
  while( idle_time != 0 ) { /* You may want to use a different condition */
    /* Get a request message. */
    
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      
      switch (_ENDPOINT_P(msg.m_source)) {
        
        case HARDWARE: /* hardware interrupt notification */

          if (msg.m_notify.interrupts & timer_set) { /* subscribed interrupt */
            /* process it */
            
            timer_int_handler();
            
            if (int_counter % 60 == 0){
              printf(" %d \n", idle_time - 1);
              idle_time--;
            }
          }
          
          if (msg.m_notify.interrupts & mouse_set) { /* subscribed interrupt */
            /* process it */
            
            mouse_ih();
            
            if ((counter == 1) && (code & BIT1_IDENTFIER)){
              scancode[0] = code;
              counter++;
            }
            
            else if (counter == 2){
              scancode[1] = code;
              counter++;
            }

            else if (counter == 3){
              scancode[2] = code;
              counter++;
            }
            
            if (counter > 3){
              mouse_packet_builder(&pkt, scancode);
              mouse_print_packet(&pkt);
              counter = 1;
          
            }

            idle_time = idle_aux;
            int_counter = 0;

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
  
  if(timer_unsubscribe_int())
    return 1;

  hook_id = hook_id_m;

  if (sys_irqdisable(&hook_id))
    return 1;

  if (mouse_set_data_report(0))
    return 1;

  if (sys_irqenable(&hook_id))
    return 1;

  if(kbc_unsubscribe_int())
    return 1;

  return 0;
}


int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    
  int ipc_status, r;
  message msg;
  uint8_t irq_id = 0;
  struct packet pkt;
  uint8_t counter = 1;
  uint8_t scancode[3];
  static States state = INIT;

  if (mouse_subscribe_int(&irq_id))
    return 1;

  if (sys_irqdisable(&hook_id))
    return 1;

  if (mouse_set_data_report(1))
    return 1;

  if (sys_irqenable(&hook_id))
    return 1;

  int irq_set = BIT(irq_id);
  
  while( state != FINISH ) { /* You may want to use a different condition */
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
            
            mouse_ih();
            
            if ((counter == 1) && (code & BIT1_IDENTFIER)){
              scancode[0] = code;
              counter++;
            }
            
            else if (counter == 2){
              scancode[1] = code;
              counter++;
            }

            else if (counter == 3){
              scancode[2] = code;
              counter++;
            }
            
            if (counter > 3){
              
              mouse_packet_builder(&pkt, scancode);
              mouse_print_packet(&pkt);
              counter = 1;
              
              if (get_event(&pkt, &ev))
                return 1;
                          
              state_machine(&ev, x_len, tolerance, &state);
               
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

  if (sys_irqdisable(&hook_id))
    return 1;

  if (mouse_set_data_report(0))
    return 1;

  if (sys_irqenable(&hook_id))
    return 1;

  if(kbc_unsubscribe_int())
    return 1;

  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
 
  struct packet pkt;

  while (cnt != 0){

    mouse_poll(&pkt);

    mouse_print_packet(&pkt);

    cnt--;

    tickdelay(micros_to_ticks(period * 1000));
  }

  if (mouse_issue_cmd(SET_STREAM_MODE))
    return 1;

  if (mouse_set_data_report(0))
    return 1;

  uint8_t cmd = minix_get_dflt_kbc_cmd_byte();

  if (kbc_issue_cmd(KBC_WRITE_CMD))
    return 1;

  if (kbc_issue_arg(cmd))
      return 1;

  return 0;
}

