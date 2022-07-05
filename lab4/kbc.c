#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"
#include "i8042.h"

int (kbc_subscribe_int)(uint8_t *bit_no){
    
    *bit_no = hook_id;

    if(sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id))
        return 1;

    return 0;
}

int (mouse_subscribe_int)(uint8_t *bit_no){
    
    *bit_no = hook_id;

    if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id))
        return 1;

    return 0;
}

int (kbc_unsubscribe_int)(void){
    
    if(sys_irqrmpolicy(&hook_id))
        return 1;

    return 0;
}

int (kbc_int_handler)(void){
    
    uint8_t st = 0;

    if(util_sys_inb(KBC_ST_REG, &st))
        return 1;
    
    if(st & PARITY_ERROR)
        return 1;

    if(st & TIMEOUT_ERROR)
        return 1;

    if(st & OUT_BUF_STATE)
        if(util_sys_inb(OUT_BUF, &code))
            return 1;

    return 0;
}

int (kbc_issue_cmd)(uint8_t cmd){

    uint8_t st = 0;
    
    while( 1 ) {
        
        util_sys_inb(KBC_ST_REG, &st); /* assuming it returns OK */
            /* loop while 8042 input buffer is not empty */
        
        if( (st & IN_BUF_STATE) == 0 ) {
            sys_outb(KBC_CMD, cmd); /* no args command */
            return 0;
        }
    }
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
}

int (kbc_read_data)(uint8_t *byte){

    uint8_t st = 0;

    while( 1 ) {
        
        util_sys_inb(KBC_ST_REG, &st); /* assuming it returns OK */
            /* loop while 8042 output buffer is empty */
        
        if(st & OUT_BUF_STATE) {
            
            util_sys_inb(OUT_BUF, byte); /* ass. it returns OK */
            
            if ((st &(PARITY_ERROR | TIMEOUT_ERROR)) == 0 )
                return 0;
            
            else
                return -1;
        }
    }
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()  
}

int (kbc_issue_arg)(uint8_t arg){
    
    uint8_t st = 0;
    
    while( 1 ) {
        
        util_sys_inb(KBC_ST_REG, &st); /* assuming it returns OK */
            /* loop while 8042 input buffer is not empty */
        
        if( (st & IN_BUF_STATE) == 0 ) {
            sys_outb(KBC_WRITE_ARGS, arg); /* no args command */
            return 0;
        }
    }
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
}
