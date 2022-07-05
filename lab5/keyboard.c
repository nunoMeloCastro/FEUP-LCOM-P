#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"
#include "i8042.h"
#include "keyboard.h" 

int keyboard_poll(uint8_t bytes[], uint8_t *size){

    if(bytes == NULL | size == NULL)
        return 1;

    if (kbc_read_data(&code))
        return 1;
    
    if (code == TWO_BYTE_CODE){
        
        if(kbc_read_data(&bytes[1]))
            return 1;
        
        bytes[0] = code;
        *size = 2;            
    }
    
    else {
        bytes[0] = code;
        bytes[1] = 0;
        *size = 1;
    }

    return 0;
}

int keyboard_restore(){

    uint8_t cmd = 0;

    if (kbc_issue_cmd(KBC_READ_CMD))
        return 1;
    
    if (kbc_read_data(&cmd))
        return 1;

    cmd = (cmd | KB_INT) & (~ENABLE_KB);

     if (kbc_issue_cmd(KBC_WRITE_CMD))
        return 1;
    
    if (kbc_issue_arg(cmd))
        return 1;

    return 0;
}
