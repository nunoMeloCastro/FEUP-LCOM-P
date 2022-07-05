#include <lcom/lcf.h>
#include <lcom/lab4.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"
#include "i8042.h"
#include "mouse.h"

void (mouse_packet_builder)(struct packet *pp, uint8_t scode[]){
 
        pp -> bytes[0] = scode[0];
        pp -> bytes[1] = scode[1];
        pp -> bytes[2] = scode[2];
        pp -> rb       = (scode[0] & RIGHT_BUTTON);
        pp -> mb       = (scode[0] & MIDDLE_BUTTON);
        pp -> lb       = (scode[0] & LEFT_BUTTON);
        pp -> x_ov     = (scode[0] & X_OVERFLOW);
        pp -> y_ov     = (scode[0] & Y_OVERFLOW);

        if(scode[0] & DELTA_X){
            pp -> delta_x  = signed_delta(scode[1]);
        }
        else
            pp -> delta_x  = (int16_t)scode[1];
        
        if(scode[0] & DELTA_Y){
            pp -> delta_y  = signed_delta(scode[2]);
        }
        else
            pp -> delta_y  = (int16_t)scode[2];
}

void (mouse_ih)(void){
    
    uint8_t st = 0;

    if(util_sys_inb(KBC_ST_REG, &st))
        return;
    
    if(st & PARITY_ERROR)
        return;

    if(st & TIMEOUT_ERROR)
        return;

    if((st & OUT_BUF_STATE) || (st & KB_MOUSE_SEL))
        if(util_sys_inb(OUT_BUF, &code))
            return;

    return;
}

int (mouse_set_data_report)(int set){
    
    if (set){
        if (mouse_issue_cmd(ENABLE_DATA_REP))
            return 1;
    }

    else {
        if (mouse_issue_cmd(DISABLE_DATA_REP))
            return 1;
    }

    return 0;
}

int (mouse_read_byte)(uint8_t *byte){
    
    uint8_t st = 0;

    while( 1 ) {
        
        util_sys_inb(KBC_ST_REG, &st); /* assuming it returns OK */
            /* loop while 8042 output buffer is empty */
        
        if((st & OUT_BUF_STATE) && (st & KB_MOUSE_SEL)) {
            
            util_sys_inb(OUT_BUF, byte); /* ass. it returns OK */
            
            if ((st &(PARITY_ERROR | TIMEOUT_ERROR)) == 0 )
                return 0;
            
            else
                return -1;
        }
    }  
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
}


int (mouse_issue_cmd)(uint8_t cmd){

    uint8_t resp = 0;

    while( 1 ) {

        if (kbc_issue_cmd(KBC_WRITE_MOUSE))
            return 1;

        if (kbc_issue_arg(cmd))
            return 1;

        if (mouse_read_byte(&resp))
            return 1;

        if (resp == ACK_ERROR)
            return 1;

        if (resp == ACK_OK)
            return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
}

int16_t (signed_delta)(uint8_t byte){
    return (int16_t)((0xFF00) | byte);
}

int mouse_poll(struct packet *pp){
    
    uint8_t counter;
    uint8_t scancode[3];

    for(counter = 0; counter < 3; counter++){

        if(mouse_issue_cmd(MOUSE_READ_DATA))
            return 1;

        if (mouse_read_byte(&code))
            return 1;

        scancode[counter] = code;
    }

    mouse_packet_builder(pp, scancode);

    return 0;
}


