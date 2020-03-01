#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the i8042 PS/2 Mouse
 */

/**
 * @brief Subscribe and enable Mouse's interrupts
 * 
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */

int (mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribe Mouses's interrupts
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_unsubscribe_int)();

/**
 * @brief Construct a packet struct with the bits of a 3 byte packet
 * 
 * @param p address of memory to be initialized with the
 *         packet struct
 */
void build_packet_struct(struct packet *p, uint8_t *packet);

/**
 * @brief Enable the mouse
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_enable();

/**
 * @brief Disable the mouse
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_disable();

/**
 * @brief Enable Data Reporting
 * 
  * @return Return 0 upon success and non-zero otherwise
 */
int mouse_enable_data_rep();

/**
 * @brief Enable Data Reporting
 * 
  * @return Return 0 upon success and non-zero otherwise
 */
int mouse_disable_data_rep();

/**
 * @brief Write a Mouse Command(reg 0xD4) - written as argument of Kbc Write Mouse byte command
 * 
  * @return Return 0 upon success and non-zero otherwise
 */
int mouse_write_command(uint8_t cmd);

/**
 * @brief Check the acknowledgment byte sent by the mouse controller
 *  
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_check_ack(uint8_t *ackB);

/**@brief Mouse event enumeration for the actual state*/
 
typedef enum{
    INIT,
    DRAWDOWN,
    VERTEX,
    DRAWUP,
}state_t ;

/**@brief Mouse event enumeration type, relevant for gesture detection*/
typedef enum{
    LEFT_DOWN,
    LEFT_UP,
    RIGHT_DOWN,
    RIGHT_UP,
    INVALID
}ev_type_t;

/**
 * @brief Detects mouse event for mouse_test_gesture()
 * 
 * @param pp address of a struct packet with a mouse packet (already parsed)
 * @param evt address of the mouse event detected (updated in the function)
 */
void mouse_check_event(	struct packet *pp, ev_type_t *evt);

/**
 * @brief Handles the mouse movements and checks if the desired movement has been completed
 * 
 * @param ev address of a struct packet with a mouse packet (already parsed)
 * @param evt mouse event
 * @param ev address of a struct packet with a mouse packet (already parsed)
 * @param evt mouse event
 * @return Return 0 if the movement was completed and 1 otherwise
 */
int mouse_handle_event(struct packet *pp, ev_type_t evt,  uint8_t x_len, uint8_t tolerance);

#endif /* __MOUSE_H */
