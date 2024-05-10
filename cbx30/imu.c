/** @file imu.c
*
* @brief Functions for interfacing the LSM6DSL IMU peripheral.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <bsp/imu.h>
#include <blspi-gpio.h>
#include <blspi.h>
#include <blspi-master.h>
#include <blspi-stm32-hal.h>
#include <blspi-gpio.h>
#include <blgpio.h>
#include <blgpio-stm32-hal.h>

//-------------------------------- MACROS -------------------------------------
#define     PIN_ACC_MOSI    BLGPIO_STM32_GPIO_ID('E', 15)
#define     PIN_ACC_MISO    BLGPIO_STM32_GPIO_ID('E', 14)
#define     PIN_ACC_CS      BLGPIO_STM32_GPIO_ID('B', 12)
#define     PIN_ACC_CLK     BLGPIO_STM32_GPIO_ID('E', 13)

#define     PIN_ACC_IN      BLGPIO_STM32_GPIO_ID('B', 11)

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//----------------------- STATIC DATA & CONSTANTS -----------------------------
static blspi_master_t master0;
static blspi_stm32_hal_t hspi1;

static blspi_device_t dev0 = {
        .cs = PIN_ACC_CS,
        .config = {
                .mode = 0,
                .speed_hz = 500000,
                .lsb_ordering = false,
        },
};

static uint8_t init_flag = 0;
static uint8_t init_stat = 0;

//------------------------------ GLOBAL DATA ----------------------------------
uint8_t bsp_imu_spi_init ()
{
    blspi_error_t ret = BLSPI_ERROR_OK;

    if (!init_flag)
    {
        if (BLSPI_ERROR_OK !=
            blspi_stm32_hal_init(&master0, &hspi1, PIN_ACC_MOSI,
                                 PIN_ACC_MISO, PIN_ACC_CLK))
        {
            ret = BLSPI_ERROR_NO_DEVICE;
        }

        if (BLSPI_ERROR_OK != blspi_dev_register(&dev0, &master0))
        {
            ret = BLSPI_ERROR_UNKNOWN;
        }

        init_flag = 1;
        init_stat = ret;
    }

    if (init_flag)
    {
        ret = init_stat;
    }

    return ret;
}

uint8_t bsp_imu_spi_read (uint8_t reg_addr, uint8_t *p_buff, uint16_t buff_len)
{
    blspi_error_t ret = BLSPI_ERROR_OK;

    uint8_t addr = (uint8_t)(0x80 | (reg_addr & 0x7F));

    blspi_transfer_t conversation[] = {
            {
                    .blk_out = &addr, // send from buf
                    .len = 1,
            },
            {
                    .blk_in = p_buff,  // receive into buf
                    .len = buff_len,
            },
    };

    ret = blspi_dev_transfer_multiple(&dev0, conversation, 2);

    return ret;
}

uint8_t bsp_imu_spi_write (uint8_t reg_addr, uint8_t *p_buff, uint16_t buff_len)
{
    blspi_error_t ret = BLSPI_ERROR_OK;

    blspi_transfer_t conversation[] = {
            {
                    .blk_out = &reg_addr, // send from buf
                    .len = 1,
            },
            {
                    .blk_out = p_buff,  // receive into buf
                    .len = buff_len,
            },
    };

    ret = blspi_dev_transfer_multiple(&dev0, conversation, 2);

    return ret;
}

//---------------------------- PUBLIC FUNCTIONS -------------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//--------------------------- INTERRUPT HANDLERS ------------------------------
