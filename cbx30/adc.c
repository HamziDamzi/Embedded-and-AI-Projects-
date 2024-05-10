/** @file adc.c
*
* @brief Initialization and reading of ADC.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <bsp/adc.h>
#include <stm32l4xx_hal.h>
#include <error_handler.h>
#include <RTT.h>
#include <inc/bsp/bsp.h>
#include <blog.h>

//-------------------------------- MACROS -------------------------------------
#define ADC_PIN         GPIO_PIN_3
#define ADC_PORT        GPIOF
#define ADC_INSTANCE    ADC3
#define ADC_CHANNEL     ADC_CHANNEL_6
#define ADC_SAMPLES     (10u)
#define ADC_FULL_SCALE  (4096u)
#define ADC_REF         (3300u)
#define ADC_OFFSET      (0u)
#define ADC_DIV         (1.68)

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//----------------------- STATIC DATA & CONSTANTS -----------------------------
static ADC_HandleTypeDef hndl_adc1;

//------------------------------ GLOBAL DATA ----------------------------------

//---------------------------- PUBLIC FUNCTIONS -------------------------------
void bsp_adc_init (void)
{
    ADC_ChannelConfTypeDef sConfig;

    /* ADC Periph clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    /* ADC Channel GPIO pin configuration */
    GPIO_InitStruct.Pin  = ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_PORT, &GPIO_InitStruct);

    hndl_adc1.Instance                    = ADC_INSTANCE;
    hndl_adc1.Init.ClockPrescaler         = ADC_CLOCK_ASYNC_DIV256;
    hndl_adc1.Init.Resolution             = ADC_RESOLUTION_12B;
    hndl_adc1.Init.DataAlign              = ADC_DATAALIGN_RIGHT;
    hndl_adc1.Init.ScanConvMode           = ADC_SCAN_DISABLE;
    hndl_adc1.Init.EOCSelection           = ADC_EOC_SINGLE_CONV;
    hndl_adc1.Init.LowPowerAutoWait       = DISABLE;
    hndl_adc1.Init.ContinuousConvMode     = DISABLE;
    hndl_adc1.Init.NbrOfConversion        = 1;
    hndl_adc1.Init.DiscontinuousConvMode  = DISABLE;
    hndl_adc1.Init.NbrOfDiscConversion    = 1;
    hndl_adc1.Init.ExternalTrigConv       = ADC_SOFTWARE_START;
    hndl_adc1.Init.ExternalTrigConvEdge   = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hndl_adc1.Init.DMAContinuousRequests  = DISABLE;
    hndl_adc1.Init.Overrun                = ADC_OVR_DATA_PRESERVED;
    hndl_adc1.Init.OversamplingMode       = DISABLE;

    if (HAL_OK != HAL_ADC_Init(&hndl_adc1))
    {
        BL_RAISE_ERROR("ADC Init failed", EHALADC);
        EPRINT("ADC Init failed.\n");
    }

    /**Configure Regular Channel
    */
    sConfig.Channel      = ADC_CHANNEL;
    sConfig.Rank         = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_92CYCLES_5;
    sConfig.SingleDiff   = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset       = 0;

    if (HAL_OK != HAL_ADC_ConfigChannel(&hndl_adc1, &sConfig))
    {
        BL_RAISE_ERROR("ADC channel config failed", EHALADC);
        EPRINT("ADC channel config failed\n");
    }

    if (HAL_OK != HAL_ADCEx_Calibration_Start(&hndl_adc1, ADC_SINGLE_ENDED))
    {
        BL_RAISE_ERROR("ADC calibration failed", EHALADC);
        EPRINT("ADC calibration failed\n");
    }
}

int32_t bsp_adc_val_get (void)
{
    int32_t adc_value = 0;
    HAL_ADC_Start(&hndl_adc1);

    if (HAL_OK == HAL_ADC_PollForConversion(&hndl_adc1, 1000))
    {
        adc_value = HAL_ADC_GetValue(&hndl_adc1);
    }
    else
    {
        adc_value = -1;
        dprintf("ADC TIMEOUT ERROR!");
        EPRINT("ADC TIMEOUT ERROR!");
    }

    HAL_ADC_Stop(&hndl_adc1);
    return adc_value;
}

/**
 * @brief Gets voltage on VBAT pin
 * @return Battery voltage in mV
 */
uint16_t bsp_battery_voltage_get (void)
{
    uint32_t adc_value = 0;
    uint32_t vbat;
    int32_t temp_adc;
    uint8_t cnt_avg = 0;

    // Take multiple ADC samples.
    adc_value = 0;
    cnt_avg = 0;
    for (int adc_cnt = 0; adc_cnt < ADC_SAMPLES; adc_cnt++)
    {
        temp_adc = bsp_adc_val_get();

        if (temp_adc > 0)
        {
            adc_value += temp_adc;
            cnt_avg++;
        }
        else
        {
            bsp_delay_ms(3);
        }
    }

    // Calculate the average value of collected samples.
    if (cnt_avg)
    {
        adc_value /= cnt_avg;
    }
    else
    {
        adc_value = 0;
    }


    // Convert ADC value to mV.
    vbat = (uint32_t)(((float)adc_value * (float)ADC_REF * ADC_DIV) /
                      ((float)ADC_FULL_SCALE)) + ADC_OFFSET;

    return (uint16_t) vbat;
}
//--------------------------- PRIVATE FUNCTIONS -------------------------------

//--------------------------- INTERRUPT HANDLERS ------------------------------
