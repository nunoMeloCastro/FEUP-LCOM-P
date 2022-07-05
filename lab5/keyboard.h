#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>

int keyboard_poll(uint8_t bytes[], uint8_t *size);

int keyboard_restore(void);

#endif /* _KEYBOARD_H */
