/** @file led_gestures.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_LED_GESTURES_H
#define CROSSBOX_LED_GESTURES_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
void led_gestures_heart_beat (void);
void led_gestures_turn_on (void);
void led_gestures_gsp_found (void);
void led_gestures_charging (void);
void led_gestures_init (void);

#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_LED_GESTURES_H

