/** @file bsp_button.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef __BSP_BUTTON_H__
#define __BSP_BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>
#include <blbutton.h>
#include <blbutton-key.h>

//-------------------------- CONSTANTS & MACROS -------------------------------
extern blbtn_key_t key;

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Initialize button GPIO using HAL libs.
 */
void bsp_button_init_raw ();

/**
 * @brief Get current state of button.
 * @return 1 -> button pressed
 *         0 -> button not pressed
 */
uint8_t bsp_button_get_state ();

/**
 * @brief Initialize button pin.
 */
void bsp_button_init();

#ifdef __cplusplus
}
#endif

#endif //__BSP_BUTTON_H__

