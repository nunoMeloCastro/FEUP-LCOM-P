#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define DELAY_US                20000

/* IRQ LINE */ 

#define KBC_IRQ                 1       /* @brief KBC IRQ line */
#define MOUSE_IRQ               12      /* @brief MOUSE IRQ line */

/* I/O PORTS */

#define OUT_BUF                 0x60    /* KBC output buffer address */
#define KBC_WRITE_ARGS          0x60    /* Write KBC command arguments */
#define KBC_ST_REG              0x64    /* Read KBC state */
#define KBC_CMD                 0x64    /* Write KBC commands*/

/* KBC COMMANDS */

#define KBC_READ_CMD            0x20    /* @brief Read KBC command byte */
#define KBC_WRITE_CMD           0x60    /* @brief Write KBC command byte */
#define KBC_SELF_TEST           0xAA    /* @brief KBC self diagnostic */
#define KBC_INTERFACE_TEST      0xAB    /* @brief KBC interface diagnostic */
#define KBC_DISABLE_KBC         0xAD    /* @brief Disable KBC */
#define KBC_ENABLE_KBC          0xAE    /* @brief Enable KBC */
#define KBC_READ_IN_PORT        0xC0    /* @brief Read input port */
#define KBC_READ_OUT_PORT       0xD0    /* @brief Read output port */
#define KBC_WRITE_OUT_PORT      0xD1    /* @brief Write output port */
#define KBC_READ_TEST_INP       0xE0    /* @brief Read test inputs */
#define KBC_SYSTEM_RES          0xFE    /* @brief System Reset */

/* MOUSE COMMANDS */

#define KBC_DISABLE_MOUSE       0xA7    /* @brief Disable mouse port */
#define KBC_ENABLE_KB           0xA8    /* @brief Enable mouse port */
#define KBC_TEST_MOUSE          0xA9    /* @brief Mouse port diagnostic */
#define KBC_WRITE_MOUSE         0xD4    /* @brief Write byte to mouse */

/* STATUS BYTE MASKS */

#define OUT_BUF_STATE           BIT(0)  /* @brief Output buffer state */
#define IN_BUF_STATE            BIT(1)  /* @brief Input buffer state */
#define SYS_FLAG                BIT(2)  /* @brief System flag */
#define DATA_CMD_SEL            BIT(3)  /* @brief Type of byte in the input buffer selector */
#define KB_LOCK                 BIT(4)  /* @brief Keyboard lock */
#define KB_MOUSE_SEL            BIT(5)  /* @brief Keyboard or mouse data selector */
#define TIMEOUT_ERROR           BIT(6)  /* @brief Timeout error */
#define PARITY_ERROR            BIT(7)  /* @brief Parity error */

/* COMMAND BYTE MASKS */

#define KB_INT                  BIT(0)  /* @brief Enable keyboard interrupt */
#define MOUSE_INT               BIT(1)  /* @brief Enable mouse interrupt */
#define SYS_FLAG                BIT(2)  /* @brief System flag */
#define IGNORE_LOCK             BIT(3)  /* @brief Ignore keyboard lock */
#define ENABLE_KB               BIT(4)  /* @brief Enable keyboard */
#define ENABLE_MOUSE            BIT(5)  /* @brief Enable mouse */
#define TRANSLATE_CODE          BIT(6)  /* @brief Translate keyboard scancodes */
#define CMD_BIT7                BIT(7)  /* @brief Unused bit */

/* SCANCODE MACROS */

#define ESC_BRK_CODE            0x81    /* ESC break code */
#define MAKE_BRK_SEL            BIT(7)  /* Make or brake code selector bit || 0 if 1 byte or 1 if 2 bytes */ 
#define TWO_BYTE_CODE           0xE0    /* First byte of a two sized byte code */

#endif /* _LCOM_I8042_H_ */

