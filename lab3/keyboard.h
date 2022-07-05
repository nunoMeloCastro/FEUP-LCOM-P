#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <stdint.h>

uint8_t scode;
int hook_id;
int kbc_erro;

int (kb_subscribe_int)(uint8_t *bit_no);

int (kb_unsubscribe_int)(void);

//int (kb_int_handler)(void);
void (kbc_ih)(void);

int (kb_poll)(uint8_t bytes[], uint8_t *size);

int (kb_restore)(void);

int (kbc_issue_cmd)(uint8_t cmd);

int (kbc_issue_arg)(uint8_t arg);

int (kbc_read_byte)(uint8_t *byte);


#endif /* _LCOM_KEYBOARD_H_ */
