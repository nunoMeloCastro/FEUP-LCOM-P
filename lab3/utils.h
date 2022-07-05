#ifndef _LCOM_UTILS_H_
#define _LCOM_UTILS_H_

#include <stdint.h>

int (util_get_LSB)(uint16_t val, uint8_t *lsb);

int (util_get_MSB)(uint16_t val, uint8_t *msb);

#ifdef LAB3
    uint32_t sys_inb_counter;
#endif


int (util_sys_inb)(int port, uint8_t *byte);

#endif /* _LCOM_UTILS_H_ */
