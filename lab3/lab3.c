#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include "keyboard.h"
#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "utils.h"
#include "timer.c"


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int(kbd_test_scan)() {
int ipc_status;
int r;
message msg;
uint8_t id = 0;
int irq_set=BIT(id);

uint8_t size=0;
uint8_t bytes[2];

if(kb_subscribe_int(&id)!=0)
  return 1;

while(scode != ESC_BRK_CODE ){    //ALTERAR CONDIÇÃO

  if((r= driver_receive(ANY, &msg, &ipc_status)) != 0 ){
   printf("driver_receive failed with: %d", r);
    continue;

  }
  if (is_ipc_notify(ipc_status)) { /* received notification */
    switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & irq_set) { /* subscri ... process*/
          //CHAMADA DO INTERRUPT HANDLER
          
          //kb_int_handler();
          kbc_ih();
          //Constrói o scancode lido por ih (1 ou 2 bytes)
    
          if (scode == TWO_BYTE_CODE){
            size = 2;
            bytes[0] = scode;
          }
          else{
            if(size == 2){
              bytes[1] = scode;
              
            }
            else{

              bytes[0] = scode;
              size = 1;
            }
          }
          kbd_print_scancode(!(MAKE_BRK_SEL & scode),size,bytes); 

        }
        
        break;
        //imprime o scancode  
        
      default:
        break; /* no other notifications expected: do nothi*/
      }
    }
   
  }
  if (kbd_print_no_sysinb(sys_inb_counter))
    return 1;
  kb_unsubscribe_int();
  return 0;
}
 



int(kbd_test_poll)() {
  
  uint8_t bytes[2];
  uint8_t size = 0;
  
  while ( bytes[0] != ESC_BRK_CODE ){
    if (kb_poll(bytes, &size))
      return 1;
    if (kbd_print_scancode(!(bytes[size - 1] & MAKE_BRK_SEL) , size , bytes))
      return 1;
}
  if (kb_restore())
    return 1;

  if (kbd_print_no_sysinb(sys_inb_counter))
    return 1;
  
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  
int ipc_status;
int r;
message msg;
uint8_t id_k = 0;
uint8_t id_t = 0;
int hook_id_k;
int hook_id_t;
int kbd_int_bit=BIT(id_k);
int timer0_int_bit=BIT(id_t);

uint8_t size=0;
uint8_t bytes[2];
//subscrição de interrupt de keyboard 
if(kb_subscribe_int(&id_k)!=0)
  return 1;
  else{
    hook_id_k=hook_id;
  }

//subscrição de interrupt de Timer_0 
if(timer_subscribe_int(&id_t)!=0)
  return 1;
  else{
    hook_id_t=hook_id;
  }
//necessário verificar status timer_0?


while(scode != ESC_BRK_CODE && t_counter<n ){    //ALTERAR CONDIÇÃO

  if((r= driver_receive(ANY, &msg, &ipc_status)) != 0 ){
   printf("driver_receive failed with: %d", r);
    continue;

  }
  if (is_ipc_notify(ipc_status)) { // received notification 
    switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: // hardware interrupt notification 
        if (msg.m_notify.interrupts & timer0_int_bit)  // Timer0 int?
          timer_int_handler();
          
      
        if (msg.m_notify.interrupts & kbd_int_bit) { // subscri ... process
          //CHAMADA DO INTERRUPT HANDLER
        
          //kb_int_handler();
          kbc_ih();
          
          t_counter=0;
          //Constrói o scancode lido por ih (1 ou 2 bytes)
          
          if (scode == TWO_BYTE_CODE){
            size = 2;
            bytes[0] = scode;
          }
          else{
            if(size == 2)
              bytes[1] = scode;
  
            else{
              bytes[0] = scode;
              size = 1;
            }
          }
          kbd_print_scancode(!(MAKE_BRK_SEL & scode),size,bytes); 
        }
        
        break;
        //imprime o scancode  
        
      default:
        break; //no other notifications expected: do nothi
      }
    }
}
  hook_id=hook_id_k;
  kb_unsubscribe_int();
  
  hook_id=hook_id_t;
  timer_unsubscribe_int();
  return 0;
}
