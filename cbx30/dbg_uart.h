/** @file dbg_uart.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_DBG_UART_H
#define CROSSBOX_DBG_UART_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>
#include <bluart.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Initialize debug UART.
 * @param baud_rate : baud rate we want UART to set to.
 * @return 0 -> ok.
 *         1 -> fail.
 */
uint8_t bsp_dbg_uart_init (uint32_t baud_rate);

/**
 * @brief Send data to UART.
 * @param p_data : pointer to the array we want to send.
 * @param data_len : length of the data.
 * @return 0 -> ok.
 *         1 -> fail.
 */
uint8_t bsp_dbg_uart_write (char *p_data, uint16_t data_len);

/**
 * @brief Send proto data to UART.
 * @param p_data : pointer to the proto data array we want to send.
 * @param data_len : length of the data.
 * @return 0 -> ok.
 *         1 -> fail.
 */
uint8_t bsp_dbg_uart_send_proto (char *p_data, uint16_t data_len);

uint32_t bsp_dbg_read (uint8_t *p_data);

#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_DBG_UART_H

