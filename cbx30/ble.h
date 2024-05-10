/** @file bsp_ble.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_BSP_BLE_H
#define CROSSBOX_BSP_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>
#include <stdbool.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Initialize nRF52832 BLUART interface.
 * @return 1 -> ok
 *         0 -> fail
 */
uint8_t bsp_ble_init(void);

/**
 * @brief Send transmit data to BLE module.
 * @param p_str Tx buffer.
 * @param len Len of tx buffer.
 */
void bsp_ble_send (uint8_t *p_str, uint16_t len);

/**
 * @brief Reads received data.
 * @param p_data Rx buffer.
 * @param len Len of rx buffer.
 */
void bsp_ble_read (uint8_t *p_data, uint16_t len);

/**
 * @brief Ble reset function.
 * @param state 1 -> sets pin.
 *              0 -> clears pin.
 */
void bsp_ble_set_rst(uint8_t state);

/**
 * @brief Sets buffer for reading BLE.
 * @param p_buffer Rx buffer.
 * @param len Len.
 * @return 0 fine, 1 error.
 */
bool bsp_ble_set_rx(uint8_t *p_buffer, uint16_t len);

/**
 * @brief Enables BLE_USART IRQ.
 */
void bsp_ble_enable_irq(void);

/**
 * @brief Diables BLE_USART IRQ.
 */
void bsp_ble_disable_irq(void);

#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_BSP_BLE_H

