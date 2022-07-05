
#include <lcom/lcf.h>
#include "keyboard.h"
#include "utils.h"
#include "i8042.h"
#include <stdint.h>
#include <minix/sysutil.h>



int (kb_subscribe_int)(uint8_t *bit_no){
  hook_id=*bit_no;
  if(sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id) != 0)
		return 1;
 	return 0;
}

int (kb_unsubscribe_int)(void) {
	if (sys_irqrmpolicy(&hook_id) != 0 ) {
 		return 1;
	}
	return 0;
}


/*int (kb_int_handler)(void){
  uint8_t aux;
//ler ststus register do kb
  uint8_t st;
  
  if(util_sys_inb(KBC_ST_REG,&st)){
    return 1;
  }

  if(st & OUT_BUF_STATE){
   if(util_sys_inb( OUT_BUF , &aux )){
    return 1;
   }

//verificar status (possiveis erros,  mouse activado)
  if(( st &TIMEOUT_ERROR || st& PARITY_ERROR ))
    return 1;

  scode=aux; //so passa leitura valida para utilizador
  }
  return 0;
}*/

void(kbc_ih)(void){
  uint8_t aux;
//ler ststus register do kb
  uint8_t st;
  
  if(util_sys_inb(KBC_ST_REG,&st)){
    kbc_erro=1;
  }

  if(st & OUT_BUF_STATE){
   if(util_sys_inb( OUT_BUF , &aux )){
    kbc_erro=1;
   }

//verificar status (possiveis erros,  mouse activado)
  if(( st &TIMEOUT_ERROR || st& PARITY_ERROR ))
    kbc_erro=1;

  scode=aux; //so passa leitura valida para utilizador
  }
  kbc_erro=0;

}

int (kb_poll)(uint8_t bytes[], uint8_t *size){
  
  if (bytes == NULL || size == NULL)
    return 1;
  
  uint8_t code = 0;

  if (kbc_read_byte(&code))
    return 1;

  if (code == TWO_BYTE_CODE){
    if (kbc_read_byte(&bytes[1]))
      return 1;
    bytes[0] = code;
    *size = 2;
  }

  else{
    bytes[0] = code;
    bytes[1] = 0;
    *size = 1;
  }

  return 0; 
}

int (kb_restore)(){
  
  uint8_t cmd = 0;
  
  if(kbc_issue_cmd(KBC_READ_CMD))
    return 1;
  if(kbc_read_byte(&cmd))
    return 1;
  
  cmd = (cmd | KB_INT) & (~ENABLE_KB);

  if(kbc_issue_cmd(KBC_WRITE_CMD))
    return 1;
  if(kbc_issue_arg(cmd))
    return 1;
  
  return 0;
}

int (kbc_issue_cmd)(uint8_t cmd){

    uint8_t st = 0;

    while( 1 ) {
        util_sys_inb(KBC_ST_REG, &st);
        
        /* loop while 8042 input buffer is not empty */
        if( (st & IN_BUF_STATE) == 0 ) {
            sys_outb(KBC_CMD, cmd); /* no args command */
            return 0;
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
}


int (kbc_issue_arg)(uint8_t arg){

    uint8_t st = 0;

    while( 1 ) {
        util_sys_inb(KBC_ST_REG, &st);
        
        /* loop while 8042 input buffer is not empty */
        if((st & IN_BUF_STATE) == 0 ) {
            sys_outb(KBC_WRITE_ARGS, arg); /* no args command */
            return 0;
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
}

int (kbc_read_byte)(uint8_t *byte){

    uint8_t st = 0;

    while( 1 ) {
        util_sys_inb(KBC_ST_REG, &st); /* assuming it returns OK */
        
        /* loop while 8042 output buffer is empty */
        if( st & OUT_BUF_STATE ) {
            util_sys_inb(OUT_BUF, byte); /* ass. it returns OK */
            
            if ( (st & (PARITY_ERROR | TIMEOUT_ERROR)) == 0 )
                return 0;
            else
                return -1;
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
}

