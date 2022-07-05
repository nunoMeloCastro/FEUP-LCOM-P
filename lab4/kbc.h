#ifndef _KBC_H
#define _KBC_H

#include <stdint.h>

int hook_id;
uint8_t code;

int kbc_subscribe_int(uint8_t *bit_no);

int kbc_unsubscribe_int(void);

int mouse_subscribe_int(uint8_t *bit_no);

int kbc_int_handler(void);

int kbc_issue_cmd(uint8_t cmd);

int kbc_read_data(uint8_t *byte);

int kbc_issue_arg(uint8_t arg);

#endif /* _KBC_H */
