#ifndef _LCOM_I8042_H
#define _LCOM_I8042_H

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Timer. Needs to be completed.
 */

#define KBC_IRQ        1      /**<@brief Keyboard Controller IRQ line - 1*/

#define BC_ESC         0x81   /**<@brief ESC key breakcode*/
#define SCANCODE_2B_FB 0xE0   /**<@brief first byte of 2 byte scancodes*/

#define DELAY_US               20000   //delay used in the delay function

// Registers

#define OUT_BUF        0X60   /**<@brief Output Buffer */
#define STAT_REG       0x64   /**<@brief Satus Register */
#define KBC_CMD_REG    0x64   /**<@brief Where to Write Keyboard Command  */
#define KBC_CMD_ARGS   0x60   /**<@brief Where to Write the KBC arguments */

// KBC Commands

#define KBC_READ_CB        0x20   /**<@brief Read Command Byte - R: CB*/
#define KBC_WRITE_CB       0x60   /**<@brief Write Command Byte  - A: CB*/
#define KBC_SELF_TEST      0xAA   /**<@brief Self Test - R_OK: 0x55; R_ERROR: 0xFC*/
#define KBC_CHECK_KBI      0xAB   /**<@brief Check Keyboard Interface - R_OK: 0;*/
#define KBC_DIS_KBI        0xAD   /**<@brief Disable Keyboard Interface*/
#define KBC_EN_KBI         0xAE   /**<@brief Enable Keyboard Interface*/

// KBC Command Byte

#define KBC_CB_DIS2    BIT(5)  /**<@brief Disable Mouse */
#define KBC_CB_DIS     BIT(4)  /**<@brief Disable Keyboard Interface (1)*/
#define KBC_CB_INT2    BIT(1)  /**<@brief Enable OUT_BUF Interrupt,from mouse (1)*/
#define KBC_CB_INT     BIT(0)  /**<@brief Enable OUT_BUF Interrupt, from keyboard (1)*/

// Status Register

#define KB_SR_PARITY   BIT(7)  /**<@brief Parity Error (1) */
#define KB_SR_TIMEOUT  BIT(6)  /**<@brief Time Out Error (1) */
#define KB_SR_AUX      BIT(5)  /**<@brief Mode in which the controller doesn't check parity nor converts scan codes*/
#define KB_SR_INH      BIT(4)  /**<@brief Inhibit Flag (0) */
#define KB_SR_A2       BIT(3)  /**<@brief Command(1) or Data(0) byte */
#define KB_SR_SYS      BIT(2)  /**<@breif System Flag - power/reset (0), initialized (1)*/
#define KB_SR_IBF      BIT(1)  /**<@brief IN_BUF Full(1) - don't write args/commands! 0 after KBC reads it*/
#define KB_SR_OBF      BIT(0)  /**<@brief OUT_BUF Full(1) - don't write args/commands! 0 after system reads it*/

#endif // _LCOM_I8042_H

