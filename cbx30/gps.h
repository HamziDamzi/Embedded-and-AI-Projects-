/** @file custom_bsp.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef __BSP_GPS_H__
#define __BSP_GPS_H__

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
 * @brief Initialize UART for communication with GPS module.
 * @return Pointer to Bluart structure, NULL in case of Error.
 */
bluart_t * bsp_gps_uart_init(void);

/**
 * @brief Set Baud rate for serial communication
 * @param baud Baud rate in bps
 */
void bsp_gps_change_baud(uint32_t baud);

/**
 * @brief Assert reset on GPS.
 * @return void.
 */
void bsp_gps_rst_on(void);

/**
 * @brief Clear reset on GPS module.
 * @return void.
 */
void bsp_gps_rst_off(void);

#ifdef __cplusplus
}
#endif

#endif //__BSP_GPS_H__
