/** @file bsp.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2018 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef __BSP_H__
#define __BSP_H__

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
 * Initialise BSP peripherals
 */
void bsp_init(void);

/**
 * Delay for timeout milliseconds
 * @param timeout time in milliseconds
 */
void bsp_delay_ms (uint32_t timeout);

/**
 * Fetches unix timestamp and milliseconds
 * @param p_unix_timestamp location where unix timestamp is stored
 * @param p_milisec location where milliseconds value is stored
 * @return
 */
uint8_t bsp_get_timestamp(int32_t *p_unix_timestamp, uint16_t *p_milisec);

void bsp_timer_init (void);
void bsp_timer_enable(void);

/**
 * Initialise system clocks
 */
void bsp_system_clock_config (void);

/**
 * Performs device system reset
 */
void bsp_system_reset (void);

/**
 * @brief Gets unique ID of the MCU.
 * @param p_cpuid_raw : pointer to 12 byte array that will store UID.
 */
void bsp_get_cpuid_raw(uint8_t *p_cpuid_raw);

#ifdef __cplusplus
}
#endif
#endif // __BSP_H__
