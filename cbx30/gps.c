/** @file bsp_gps.c
*
* @brief BSP for GPS module.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <inc/bsp/gps.h>
#include <bluart-stm32-hal.h>
#include <blgpio.h>
#include <tps65721.h>
#include <inc/bsp/bsp.h>

//-------------------------------- MACROS -------------------------------------
#define PIN_GPS_TX                  BLGPIO_STM32_GPIO_ID('A', 0u)
#define PIN_GPS_RX                  BLGPIO_STM32_GPIO_ID('A', 1u)
#define PIN_GPS_RST                 BLGPIO_STM32_GPIO_ID('B', 2u)
#define GPS_RX_BUF_SIZE             (512u)
#define GPS_TX_BUF_SIZE             (256u)
#define RST_OVER_PIN                (0u)
#define GPS_RST_DELAY_MS            (500u)

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//----------------------- STATIC DATA & CONSTANTS -----------------------------
static bluart_t                     bluart_gps;
static bluart_stm32_hal_hw_t        bluart_gps_dev;

//------------------------------ GLOBAL DATA ----------------------------------

//---------------------------- PUBLIC FUNCTIONS -------------------------------

bluart_t * bsp_gps_uart_init(void)
{
    bluart_error_t err;
    bluart_t *p_result = NULL;

    bsp_gps_rst_on();
    bsp_delay_ms(GPS_RST_DELAY_MS);
    bsp_gps_rst_off();
    bsp_delay_ms(GPS_RST_DELAY_MS);

    err = bluart_stm32_hal_init(&bluart_gps_dev, PIN_GPS_TX,
                                PIN_GPS_RX, (-1), (-1));

    if(!err)
    {
        static uint8_t gps_buf[GPS_TX_BUF_SIZE + GPS_RX_BUF_SIZE];
        err = bluart_init(&bluart_gps, &bluart_gps_dev.hw, gps_buf, \
            GPS_TX_BUF_SIZE, GPS_RX_BUF_SIZE);
    }

    if(BLUART_ERROR_OK == err)
    {
        p_result = &bluart_gps;
    }

    return p_result;
}

void bsp_gps_change_baud(uint32_t baud)
{
    bluart_set_baudrate(&bluart_gps, baud);
}

void bsp_gps_rst_on(void)
{
#if RST_OVER_PIN
    blgpio_dir(PIN_GPS_RST, (BLGPIO_DIR_OUT | BLGPIO_DIR_FAST));
    blgpio_set(PIN_GPS_RST, false);
#else
    tps65721_ldo_off();
#endif
}

void bsp_gps_rst_off(void)
{
#if RST_OVER_PIN
    blgpio_dir(PIN_GPS_RST, (BLGPIO_DIR_OUT | BLGPIO_DIR_FAST));
    blgpio_set(PIN_GPS_RST, true);
#else
    tps65721_ldo_on();
#endif
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//--------------------------- INTERRUPT HANDLERS ------------------------------
