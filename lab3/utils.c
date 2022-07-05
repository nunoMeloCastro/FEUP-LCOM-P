#include <lcom/lcf.h>
#include "i8254.h"
#include <stdint.h>
#include "utils.h"

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb==NULL)
    return 1;
  *lsb=(uint8_t)(val);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb==NULL){
    return 1;
  }
  uint16_t temp;
  temp=(val>>8);
  *msb=(uint8_t)(temp);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *byte) {
    uint32_t st;
    #ifdef LAB3
      sys_inb_counter++;
    #endif
    if(sys_inb(port, &st)!=0)
       return 1;
    *byte=(uint8_t)st;
    return 0;
}  
