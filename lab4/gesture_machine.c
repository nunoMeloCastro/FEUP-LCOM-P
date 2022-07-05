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
#include "stdlib.h"

int get_event(struct packet *pp, Event *event){

    static uint8_t prev = 0;

    if (pp == NULL || event == NULL)
        return 1;

    uint8_t status = pp -> bytes[0];

    //if (pp -> bytes[1] != 0 || pp -> bytes[2] != 0)
    if (pp -> delta_x != 0 || pp -> delta_y != 0)

        event -> type = MOVE;

    else if  (status == (pp -> bytes[0] & BIT1_IDENTFIER)){

        if (prev == LEFT_BUTTON)
            event -> type = LB_RELEASE;

        else if (prev == RIGHT_BUTTON)
                event -> type = RB_RELEASE;
        
            else
                event -> type = OTHER;
        
        prev = 0;
    }

    else if (status == (pp -> bytes[0] & (LEFT_BUTTON | BIT1_IDENTFIER))){
        
        event -> type = LB_PRESS;
        prev = LEFT_BUTTON;

    }

    else if (status == (pp -> bytes[0] & (RIGHT_BUTTON | BIT1_IDENTFIER))){
        
        event -> type = RB_PRESS;
        prev = RIGHT_BUTTON;

    }

    else
        event -> type = OTHER;
    
    event -> x_delta = pp -> delta_x;
    event -> y_delta = pp -> delta_y;
    
    return 0;
}

/*int get_event(struct packet *pp, Event *event){

    bool prevlb = false;
    bool prevrb = false;

    if (pp == NULL || event == NULL)
        return 1;
       
    if (pp -> delta_x != 0 || pp -> delta_y != 0)
        event -> type = MOVE;
    else if(pp->rb && !prevrb)
            event -> type = RB_PRESS;
        else if(!pp->rb && prevrb)
                event -> type = RB_RELEASE;
            else if(pp->lb && !prevlb)
                    event -> type = LB_PRESS;
                else if(!pp->lb && prevlb)
                        event -> type = LB_RELEASE;
                    else
                    event -> type = OTHER;
    return 0;
}*/






void state_machine(Event *event, uint8_t x_len, uint8_t tolerance, States *state) {
    
    static int16_t x_mov = 0;
    static int16_t y_mov = 0;

    switch (*state) {
        
        case INIT:
            
            if (event -> type == LB_PRESS){
               
                *state = UP_DRAW;
            }
            break;
        
        case UP_DRAW:
            
            if (event -> type == MOVE ) {
                
                if (event -> x_delta < -tolerance || event -> y_delta < -tolerance){
                   
                    *state = INIT;
                    x_mov = 0;
                    y_mov = 0;
                    break;
                    
                }

                x_mov += event -> x_delta;
                y_mov += event -> y_delta;

            }

            else if (event -> type == LB_RELEASE ){
                
                if (x_mov < x_len || y_mov < x_mov){
                    
                    *state = INIT;
                    x_mov = 0;
                    y_mov = 0;
                    break;

                }

                *state = VERTEX;
                x_mov = 0;
                y_mov = 0;

            }

            else {

                *state = INIT;
                x_mov = 0;
                y_mov = 0;

            }
            break;
        
        case VERTEX:
            
            if (event -> type == MOVE){
                
                x_mov += event -> x_delta;
                y_mov += event -> y_delta;

                if (abs(x_mov) > tolerance || abs(y_mov) > tolerance){
                    
                    *state = INIT;
                    x_mov = 0;
                    y_mov = 0;

                }
            }

            else if (event -> type == RB_PRESS){

                *state = DOWN_DRAW;
                x_mov = 0;
                y_mov = 0;

            }

            else if (event -> type == LB_PRESS){

                *state = UP_DRAW;
                x_mov = 0;
                y_mov = 0;

            }

            else {
                
                *state = INIT;
                x_mov = 0;
                y_mov = 0; 

            }

            break;

        case DOWN_DRAW:

            if (event -> type == MOVE){

                if (event -> x_delta < -tolerance || event -> y_delta > tolerance){
                    
                    *state = INIT;
                    x_mov = 0;
                    y_mov = 0;
                    break;

                }

                x_mov += event -> x_delta;
                y_mov += event -> y_delta;

            }

            else if (event -> type == RB_RELEASE){
                
                if (x_mov == 0 || y_mov == 0){

                    *state = INIT;
                    break;

                }

                if (x_mov >= x_len || y_mov <= -x_mov){
                    
                    *state = FINISH;
                    x_mov = 0;
                    y_mov = 0;
                    break;

                }

                else {

                    *state = INIT;
                    x_mov = 0;
                    y_mov = 0;
                    
                }
            }
            
            else{  

                *state = INIT;
                x_mov = 0;
                y_mov = 0;

            }

            break;
        
        case FINISH:

            *state = EXIT;
            break;
            
        default:

            break;
    }
}
