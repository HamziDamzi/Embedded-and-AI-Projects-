/** @file bsp.c
*
* @brief BSP functions.
*
* @par
* COPYRIGHT NOTICE: (c) 2018 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <inc/bsp/bsp.h>
#include <inc/bsp/rtc.h>
#include <stm32l4xx_hal.h>
#include <blgpio.h>
#include <bluart.h>
#include <bluart-stm32-hal.h>
#include <stm32l4xx_hal_tim.h>
#include <stm32l4xx_hal_rcc.h>
#include <stm32l4xx.h>
#include <stm32l4xx_hal_gpio.h>
#include <imu_task.h>
#include <RTT.h>
#include <error_handler.h>
#include <inc/bsp/i2c.h>
#include <inc/bsp/buzzer.h>
#include <blog.h>
#include <emmc.h>
#include <inc/bsp/wifi.h>
#include <inc/bsp/dma.h>

//-------------------------------- MACROS -------------------------------------
#define PRESCALER       (4799)
#define PERIOD          (199)       // Frequency of 50Hz
#define TIM_USR_IRQ     TIM3_IRQn
#define TIM_USR         TIM3

#if 0
#define PIN_UART_TX     BLGPIO_STM32_GPIO_ID('A', 2u)
#define PIN_UART_RX     BLGPIO_STM32_GPIO_ID('A', 3u)
#else
#define PIN_UART_TX     BLGPIO_STM32_GPIO_ID('C', 4)
#define PIN_UART_RX     BLGPIO_STM32_GPIO_ID('C', 5)
#endif
#define UART_TX_BUFF    (128u)
#define UART_RX_BUFF    (128u)
#define UART_BAUD       (38400u)

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

static void system_clock_setup_48MHz (void);
static void system_clock_setup_80MHz(void);
//----------------------- STATIC DATA & CONSTANTS -----------------------------

static TIM_HandleTypeDef struct1;

//------------------------------ GLOBAL DATA ----------------------------------
extern void _Error_Handler(char *file, int line);

//---------------------------- PUBLIC FUNCTIONS -------------------------------

void bsp_init (void) {
    bsp_system_clock_config();

    HAL_Init();

    // Enable master clock on IO2.
    HAL_PWREx_EnableVddIO2();

    blgpio_init();
    bloswrap_init();

    bsp_rtc_init();

    bsp_i2c_init();

    bsp_buzzer_pwm_init();

    // Enable clock on all GPIO Ports.
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    // Init eMMC early to check for the bootloader update pending.
    emmc_init();

    if(!bsp_wifi_init())
    {
        dprintf("BSP Wifi failed\n");
    }

    bsp_dma_init();
}

void bsp_system_clock_config (void)
{
    system_clock_setup_48MHz();

    SystemCoreClockUpdate();

    /* Configure the Systick interrupt time */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

    /* Configure the Systick */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /**Enable MSI Auto calibration (LSE should be enabled already)
    */
    HAL_RCCEx_EnableMSIPLLMode();

    /* GPIO port G power */
    HAL_PWREx_EnableVddIO2();

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void bsp_delay_ms (uint32_t timeout)
{
    bool os_delay = false;
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
    os_delay = (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED);
#endif
    if (os_delay)
    {
        TickType_t ticks = timeout / portTICK_PERIOD_MS;
        ticks = ticks ? ticks : 1;
        vTaskDelay(ticks);
    }
    else
    {
        HAL_Delay(timeout);
    }
}

uint8_t bsp_get_timestamp(int32_t *p_unix_timestamp, uint16_t *p_milisec)
{
    uint8_t ret_val = 1;

    return ret_val;
}

void bsp_timer_init(void)
{
    //enable function
    bsp_timer_enable();

    //Define timer value

    struct1.Instance = TIM_USR;
    struct1.Init.Prescaler = PRESCALER;
    struct1.Init.CounterMode = TIM_COUNTERMODE_UP;
    struct1.Init.Period = PERIOD;
    struct1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    //Init timer
    HAL_TIM_Base_Init(&struct1);

    //Start timer
    HAL_TIM_Base_Start_IT(&struct1);

    //Enable interrupt
    HAL_NVIC_SetPriority(TIM_USR_IRQ, 6, 0);
    HAL_NVIC_EnableIRQ(TIM_USR_IRQ);

}

void bsp_timer_enable(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();
}

void bsp_system_reset (void)
{
    bsp_rtc_compability_mode();
    HAL_NVIC_SystemReset();
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

static void system_clock_setup_80MHz(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /**Configure LSE Drive Capability
    */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType =
            RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 3;
    RCC_OscInitStruct.PLL.PLLN = 10;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        BL_RAISE_ERROR("RCC oscillator config failed.", EHALRCC);
        EPRINT("Rcc oscilator config failed\n");
    }
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        BL_RAISE_ERROR("RCC oscillator config failed.", EHALRCC);
        EPRINT("RCC oscillator config failed.\n");
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UART4|
                                         RCC_PERIPHCLK_USB|
                                         RCC_PERIPHCLK_SDMMC1|
                                         RCC_PERIPHCLK_ADC|
                                         RCC_PERIPHCLK_I2C2|
                                         RCC_PERIPHCLK_RTC;
    PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_MSI;
    PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_MSI;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        BL_RAISE_ERROR("RCC oscillator config failed.", EHALRCC);
        EPRINT("RCC oscillator config failed.\n");
    }

    /**Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        BL_RAISE_ERROR("RCC oscillator config failed.", EHALRCC);
        EPRINT("RCC oscillator config failed.\n");
    }

    /**Enable MSI Auto calibration
    */
    HAL_RCCEx_EnableMSIPLLMode();
}

static void system_clock_setup_48MHz (void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /**Configure LSE Drive Capability
    */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType =
            RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        BL_RAISE_ERROR("RCC oscillator config failed.", EHALRCC);
        EPRINT("RCC oscillator config failed.\n");
    }
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        BL_RAISE_ERROR("RCC oscillator config failed.", EHALRCC);
        EPRINT("RCC oscillator config failed.\n");
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UART4|
                                         RCC_PERIPHCLK_USB|
                                         RCC_PERIPHCLK_SDMMC1|
                                         RCC_PERIPHCLK_ADC|
                                         RCC_PERIPHCLK_I2C2|
                                         RCC_PERIPHCLK_RTC;
    PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_MSI;
    PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_MSI;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        BL_RAISE_ERROR("RCC oscillator config failed.", EHALRCC);
        EPRINT("RCC oscillator config failed.\n");
    }

    /**Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        BL_RAISE_ERROR("RCC oscillator config failed.", EHALRCC);
        EPRINT("RCC oscillator config failed.\n");
    }

    /**Enable MSI Auto calibration
    */
    HAL_RCCEx_EnableMSIPLLMode();
}

void bsp_get_cpuid_raw(uint8_t *p_cpuid_raw)
{
    static const int                c_cpu_id_data_size_bytes = 12u;

    // UID start address for L series.
    static const uint32_t           c_cpu_id_addr = 0x1FFF7590;

    uint8_t *p_addr = (uint8_t *)c_cpu_id_addr;

    for (uint32_t adr_cnt = 0; adr_cnt < c_cpu_id_data_size_bytes; adr_cnt++)
    {
        p_cpuid_raw[adr_cnt] = *(p_addr + adr_cnt);
    }
}
//--------------------------- INTERRUPT HANDLERS ------------------------------
void TIM3_IRQHandler (void)
{
    if(__HAL_TIM_GET_FLAG(&struct1, TIM_FLAG_UPDATE) == SET)
    {
        imu_task_hw_timer_cb();
        __HAL_TIM_CLEAR_FLAG(&struct1, TIM_FLAG_UPDATE);

    }

}
