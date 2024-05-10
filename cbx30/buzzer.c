/** @file buzzer.c
*
* @brief Buzzer functions.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <inc/bsp/buzzer.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <RTT.h>
#include <stm32l4xx_hal.h>
#include <stm32l4xx_hal_tim.h>
//-------------------------------- MACROS -------------------------------------

#define BUZZER_TIMER_HANDLER    (htim4)
#define BUZZER_TIMER_CHANNEL    (TIM_CHANNEL_2)
#define BUZZER_TIMER_INSTANCE   (TIM4)
#define BUZZER_TIMER_BASE_FREQ  (5000000u)

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//----------------------- STATIC DATA & CONSTANTS -----------------------------

static TIM_HandleTypeDef htim4;
static uint8_t buzz_pwm_dycle = 50u;

//------------------------------ GLOBAL DATA ----------------------------------

extern void HAL_TIM_MspPostInit (TIM_HandleTypeDef *htim);

//---------------------------- PUBLIC FUNCTIONS -------------------------------

void bsp_buzzer_pwm_init (void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;

    BUZZER_TIMER_HANDLER.Instance = TIM4;
    BUZZER_TIMER_HANDLER.Init.Prescaler = 48;
    BUZZER_TIMER_HANDLER.Init.CounterMode = TIM_COUNTERMODE_UP;
    BUZZER_TIMER_HANDLER.Init.Period = 0;
    BUZZER_TIMER_HANDLER.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&BUZZER_TIMER_HANDLER) != HAL_OK)
    {
        dprintf("Init failed\n");
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&BUZZER_TIMER_HANDLER, &sClockSourceConfig) != HAL_OK)
    {
        dprintf("Init failed\n");
    }

    if (HAL_TIM_PWM_Init(&BUZZER_TIMER_HANDLER) != HAL_OK)
    {
        dprintf("Init failed\n");
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&BUZZER_TIMER_HANDLER, &sMasterConfig) != HAL_OK)
    {
        dprintf("Init failed\n");
    }

    memset((void *) &sConfigOC, 0, sizeof(sConfigOC));
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&BUZZER_TIMER_HANDLER, &sConfigOC, BUZZER_TIMER_CHANNEL) != HAL_OK)
    {
        dprintf("Init failed\n");
    }

    HAL_TIM_MspPostInit(&BUZZER_TIMER_HANDLER);
}

void bsp_buzzer_pwm_start (uint16_t frequency)
{
    TIM_HandleTypeDef *p_htim = &BUZZER_TIMER_HANDLER;

    if (0 < frequency)
    {
        uint32_t period = BUZZER_TIMER_BASE_FREQ / frequency;

        p_htim->Instance = BUZZER_TIMER_INSTANCE;
        p_htim->Init.Prescaler = ((
            SystemCoreClock / (2 * BUZZER_TIMER_BASE_FREQ) - 1));
        p_htim->Init.Period = period;
        p_htim->Init.CounterMode = TIM_COUNTERMODE_UP;
        p_htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        HAL_TIM_Base_Init(p_htim);

        TIM_OC_InitTypeDef sConfigOC;
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = (uint16_t) ((buzz_pwm_dycle * period) / 100u);
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; // duty cycle is high
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        HAL_TIM_PWM_ConfigChannel(p_htim, &sConfigOC, BUZZER_TIMER_CHANNEL);
        HAL_TIM_PWM_Start(p_htim, BUZZER_TIMER_CHANNEL);
    }
}

void bsp_buzzer_pwm_stop (void)
{
    HAL_TIM_PWM_Stop(&BUZZER_TIMER_HANDLER, BUZZER_TIMER_CHANNEL);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//--------------------------- INTERRUPT HANDLERS ------------------------------
