#ifndef _MOUSE_H
#define _MOUSE_H

#include <lcom/lcf.h>
#include <lcom/lab4.h>

#include <stdint.h>
#include <stdio.h>

void mouse_packet_builder(struct packet *pp, uint8_t scode[]);

int mouse_set_data_report(int set);

int mouse_read_byte(uint8_t *byte);

int mouse_issue_cmd(uint8_t cmd);

int16_t (signed_delta)(uint8_t byte);

int mouse_poll(struct packet *pp);

#endif /* _MOUSE_H */
