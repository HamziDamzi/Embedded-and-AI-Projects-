/** @file bsp_ble.c
*
* @brief Functions for interfacing nRF52832 BLE peripheral.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <inc/bsp/ble.h>
#include <blgpio.h>
#include <bluart-stm32-hal.h>
#include <bluart.h>
#include <RTT.h>
#include <stm32l4xx_hal_gpio.h>
#include <stm32l4xx_hal_uart.h>
#include <nrf.h>
#include <transport/ser_phy/ser_phy.h>
#include <inc/bsp/bsp.h>

//-------------------------------- MACROS -------------------------------------

#define NRF_BLE_RST_PIN     GPIO_PIN_3
#define NRF_BLE_RST_PORT    GPIOE

#define BLE_USART_TX_PIN    GPIO_PIN_6
#define BLE_USART_RX_PIN    GPIO_PIN_7
#define BLE_USART_CTS_PIN   GPIO_PIN_4
#define BLE_USART_RTS_PIN   GPIO_PIN_3
#define BLE_USART_PORT      GPIOB
#define BLE_USART_BAUD      (1000000u)

#define BLE_SER_IRQ_PRIO        (6u)
#define BLE_SER_IRQ_SUB_PRIO    (0u)
#define BLE_SER_IRQ_HANDLE      USART1_IRQn
#define SER_UART_IRQ  USART1_IRQHandler

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

bool bluart_stm32_hal_TxCpltCallback (UART_HandleTypeDef *huart);
bool bluart_stm32_hal_RxCpltCallback (UART_HandleTypeDef *huart);

//----------------------- STATIC DATA & CONSTANTS -----------------------------

UART_HandleTypeDef huart1;

//------------------------------ GLOBAL DATA ----------------------------------

//---------------------------- PUBLIC FUNCTIONS -------------------------------

uint8_t bsp_ble_init(void)
{
    uint8_t is_ok = true;
    GPIO_InitTypeDef uart_gpio;

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // TX
    uart_gpio.Pin =  BLE_USART_TX_PIN;
    uart_gpio.Mode = GPIO_MODE_AF_PP;
    uart_gpio.Pull = GPIO_PULLUP;
    uart_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    uart_gpio.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(BLE_USART_PORT, &uart_gpio);

    // Rx
    uart_gpio.Pin =  BLE_USART_RX_PIN;
    uart_gpio.Mode = GPIO_MODE_AF_PP;
    uart_gpio.Pull = GPIO_PULLUP;
    uart_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    uart_gpio.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(BLE_USART_PORT, &uart_gpio);

    // CTS (in)
    uart_gpio.Pin =  BLE_USART_CTS_PIN ;
    uart_gpio.Mode = GPIO_MODE_AF_PP;
    uart_gpio.Pull = GPIO_PULLDOWN;
    uart_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    uart_gpio.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(BLE_USART_PORT, &uart_gpio);

    // RTS (out)
    uart_gpio.Pin =   BLE_USART_RTS_PIN;
    uart_gpio.Mode = GPIO_MODE_AF_PP;
    uart_gpio.Pull = GPIO_PULLDOWN;
    uart_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    uart_gpio.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(BLE_USART_PORT, &uart_gpio);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = BLE_USART_BAUD;
    huart1.Init.WordLength = UART_WORDLENGTH_9B;

    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_EVEN;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;

    if (HAL_OK != HAL_UART_Init(&huart1))
    {
        is_ok = false;
    }

    HAL_NVIC_SetPriority(BLE_SER_IRQ_HANDLE, BLE_SER_IRQ_PRIO, BLE_SER_IRQ_SUB_PRIO);
    HAL_NVIC_EnableIRQ(BLE_SER_IRQ_HANDLE);

    __HAL_UART_DISABLE_IT(&huart1, UART_IT_ERR);

    return (uint8_t)is_ok;
}

void bsp_ble_send (uint8_t *p_str, uint16_t len)
{

    if ((NULL != p_str) && (0 < len))
    {
        HAL_UART_Transmit_IT(&huart1, p_str, len);
    }
}


void bsp_ble_set_rst(uint8_t state)
{
    GPIO_InitTypeDef nrf_rst_gpio;
    nrf_rst_gpio.Pin =  NRF_BLE_RST_PIN;
    nrf_rst_gpio.Mode = GPIO_MODE_OUTPUT_PP;
    nrf_rst_gpio.Pull = GPIO_PULLDOWN;
    nrf_rst_gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(NRF_BLE_RST_PORT, &nrf_rst_gpio);

    if (state)
    {
        // Set reset
        HAL_GPIO_WritePin(NRF_BLE_RST_PORT, NRF_BLE_RST_PIN, GPIO_PIN_SET);
    }
    else
    {
        // Clear reset.
        HAL_GPIO_WritePin(NRF_BLE_RST_PORT, NRF_BLE_RST_PIN, GPIO_PIN_RESET);
    }
}

bool bsp_ble_set_rx(uint8_t *p_buffer, uint16_t len)
{
    return (bool)HAL_UART_Receive_IT(&huart1, p_buffer, len);
}

void bsp_ble_enable_irq(void)
{
    HAL_NVIC_SetPriority(BLE_SER_IRQ_HANDLE, BLE_SER_IRQ_PRIO, BLE_SER_IRQ_SUB_PRIO);
    HAL_NVIC_EnableIRQ(BLE_SER_IRQ_HANDLE);
}

void bsp_ble_disable_irq(void)
{
    HAL_NVIC_DisableIRQ(BLE_SER_IRQ_HANDLE);
}
//--------------------------- PRIVATE FUNCTIONS -------------------------------

//--------------------------- INTERRUPT HANDLERS ------------------------------

void SER_UART_IRQ(void)
{
    HAL_UART_IRQHandler(&huart1);
}

bool bluart_stm32_hal_TxCpltCallback(UART_HandleTypeDef *huart)
{
    bool is_ok = false;

    if (USART1 == huart->Instance)
    {
        uart_ser_callback_tx(huart);
        is_ok = true;
    }

    return is_ok;
}

bool bluart_stm32_hal_RxCpltCallback(UART_HandleTypeDef *huart)
{
    bool is_ok = false;

    if (USART1 == huart->Instance)
    {
        uart_ser_callback_rx(huart);
        is_ok = true;
    }

    return is_ok;
}
