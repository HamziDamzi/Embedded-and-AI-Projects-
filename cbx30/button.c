/** @file bsp_button.c
*
* @brief Initialize button pin.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <inc/bsp/button.h>
#include <blbutton.h>
#include <blbutton-key.h>
#include <blgpio.h>
#include <stm32l4xx_hal.h>
#include <stm32l4xx_hal_gpio.h>

//-------------------------------- MACROS -------------------------------------
#define PIN_BUTTON      BLGPIO_PIN(0,7u,1u)

#define BUTTON_PORT     GPIOH
#define BUTTON_PIN      GPIO_PIN_1

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//----------------------- STATIC DATA & CONSTANTS -----------------------------
GPIO_InitTypeDef GPIO_InitStruct;

//------------------------------ GLOBAL DATA ----------------------------------

//---------------------------- PUBLIC FUNCTIONS -------------------------------
void bsp_button_init_raw ()
{
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /* ADC Channel GPIO pin configuration */
    GPIO_InitStruct.Pin  = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
}

uint8_t bsp_button_get_state ()
{
    return HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) ? 0 : 1;
}

void bsp_button_init()
{
    blbtn_init();
    blbtn_key_init(&key, PIN_BUTTON, BLGPIO_DIR_PULL_UP, true);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//--------------------------- INTERRUPT HANDLERS ------------------------------
