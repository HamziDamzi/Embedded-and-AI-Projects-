/** @file adc.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_ADC_H
#define CROSSBOX_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Initialize ADC pin and ADC peripheral
 * @return none
 */
void bsp_adc_init (void);

/**
 * @brief Function that polls ADC value
 * @return ADC conversion value, or -1 if timeout.
 */
int32_t bsp_adc_val_get (void);

/**
 * @brief Gets voltage on VBAT pin
 * @return Battery voltage in mV
 */
uint16_t bsp_battery_voltage_get (void);

#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_ADC_H

