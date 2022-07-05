#ifndef _LCOM_VBE_MACROS_H_
#define _LCOM_VBE_MACROS_H_

#include <lcom/lcf.h>

#define AH_REG                  0X4F
#define SET_LIN_FR_BUF_MODEL    BIT(14)

/* STANDARD BIOS SERVICES */

#define VIDEO_CARD              0x10
#define PC_CONFIG               0x11
#define MEM_CONFIG              0x12
#define KEYBOARD                0x16

/* ERROR CODES */

#define FUNC_CALL_FAIL          0x01    /* @brief Function call failed */
#define FUNC_NOT_SUP            0x02    /* @brief Function is not supported in current HW configuration */
#define FUNC_INVALID            0x03    /* @brief Function is invalid in current video mode */ 

/* GRAPHICS MODES */

#define IND_1024x768            0x105   /* @brief 8 bits per pixel */
#define DIRC_640x480            0x110   /* @brief 15 bits per pixel ((1:)5:5:5) */
#define DIRC_800x600            0x115   /* @brief 24 bits per pixel (8:8:8) */
#define DIRC_1280x1024          0x11A   /* @brief 16 bits per pixel (5:6:5) */
#define DIRC_1152x864           0x14C   /* @brief 32 bits per pixel ((8:)8:8:8) */

/* VBE FUNCTIONS */

#define SET_MODE                0x02    /* @brief Set VBE Mode */
#define VBE_INFO_MODE           0x01    /* @brief Return VBE Mode Information */
#define VBE_CTRL_INFO           0x00    /* @brief Return VBE Controller Information */

#endif /* _LCOM_VBE_MACROS_H_ */
