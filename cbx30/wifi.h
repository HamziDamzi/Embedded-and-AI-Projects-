/** @file wifi.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_BSP_WIFI_H
#define CROSSBOX_BSP_WIFI_H


//------------------------------ INCLUDES -------------------------------------

#include <stdbool.h>
#include <stdint.h>

#include <stm32l4xx_ll_usart.h>
//-------------------------- CONSTANTS & MACROS -------------------------------

#define     WIFI_DEFAULT_BAUD    (115200u)

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------

/**
 * Custom IRQ Handler for bypassing HAL code
 * @param huart  wifi usart instance
 */
void bsp_wifi_USART3_IRQHandler(UART_HandleTypeDef *huart);

/**
 * Initialises wifi module
 */
bool bsp_wifi_init(void);

/**
 * Turns wifi module on
 */
void bsp_wifi_turn_off(void);

/**
 * Turns wifi module off
 */
void bsp_wifi_turn_on(void);

/**
 * Reenables wifi ISR flags which get reset when changing baud rate
 */
void bsp_wifi_enable_rx_flags(void);

#endif //CROSSBOX_BSP_WIFI_H
