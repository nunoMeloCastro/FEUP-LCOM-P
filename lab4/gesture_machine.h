#ifndef _GESTURE_MACHINE_H
#define _GESTURE_MACHINE_H

#include <lcom/lcf.h>
#include <lcom/lab4.h>

#include <stdint.h>
#include <stdio.h>

typedef enum {
    
    INIT,
    UP_DRAW,
    VERTEX,
    DOWN_DRAW,
    FINISH,
    EXIT

} States;

typedef enum {
    
    OTHER,
    LB_PRESS, 
    LB_RELEASE, 
    RB_PRESS, 
    RB_RELEASE, 
    MOVE,

} type;

typedef struct {
    
    type type;
    int16_t x_delta;
    int16_t y_delta;

} Event;

Event ev;

int get_event(struct packet *pp, Event *event);

void state_machine(Event *event, uint8_t x_len, uint8_t tolerance, States *state);


#endif /* _GESTURE_MACHINE_H */
