/** @file buzzer.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef __BSP_BUZZER_H__
#define __BSP_BUZZER_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Buzzer init function, timer init.
 */
void bsp_buzzer_pwm_init (void);

/**
 * @brief Buzzer start function.
 * @param frequency Pwm freq.
 */
void bsp_buzzer_pwm_start (uint16_t frequency);

/**
 * @brief Buzzer stop function.
 */
void bsp_buzzer_pwm_stop (void);

#ifdef __cplusplus
}
#endif

#endif //__BSP_BUZZER_H__
