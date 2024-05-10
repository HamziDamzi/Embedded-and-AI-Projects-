/** @file wifi.c
*
* @brief Functions for enabling/disabling wifi ESP8285.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <inc/bsp/wifi.h>
#include <blgpio.h>
#include <blgpio-stm32-hal.h>
#include <bluart.h>
#include <bluart-stm32-hal.h>
#include <RTT.h>
#include <wifi_task.h>
//-------------------------------- MACROS -------------------------------------

#define     UART_WIFI_RX_BUF_LEN    (512u)
#define     UART_WIFI_TX_BUF_LEN    (2048u)

#define     PIN_WIFI_PWR_EN     (BLGPIO_STM32_GPIO_ID('A', 12))

// pulled up
#define     PIN_WIFI_EN         (BLGPIO_STM32_GPIO_ID('A', 11))

#define     PIN_WIFI_UART_RX         (BLGPIO_STM32_GPIO_ID('D', 9u))
#define     PIN_WIFI_UART_TX         (BLGPIO_STM32_GPIO_ID('D', 8u))
#define     PIN_WIFI_UART_CTS        (BLGPIO_STM32_GPIO_ID('B', 13u))
#define     PIN_WIFI_UART_RTS        (BLGPIO_STM32_GPIO_ID('B', 14u))

// 1 when flash booting, 0 for uart download
#define     PIN_WIFI_GPIO_0          (BLGPIO_STM32_GPIO_ID('A', 10u))

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

/**
 * Initialises wifi module
 * @return true on success, false otherwise
 */
static bool bsp_wifi_module_init(void);

/**
 * Initialises uart connection
 * @return true on success, false otherwise
 */
static bool bsp_wifi_uart_init(void);

/**
 * Initialises DMA on wifi USART RX line
 */
static void bsp_wifi_dma_init(void);

/**
 * Function for overriding default bluart start_read op
 * @param hw bluart_hw_t instance
 * @return bluart error status
 */
static bluart_error_t bluart_stm32_hal_start_dma_read (bluart_hw_t *hw);

/**
 * DMA Thread for processing data copied from RX line
 * @param arg
 */
static void bsp_wifi_dma_thread(void* arg);

/**
 * Function which transfers data from DMA buffer to internal bluart buffer
 */
static void bsp_wifi_process_dma_data(void);

//----------------------- STATIC DATA & CONSTANTS -----------------------------
static bluart_stm32_hal_hw_t bluartstmhw0;
static bluart_hw_ops_t wifi_uart_ops;

static char uart_wifi_buf[UART_WIFI_RX_BUF_LEN + UART_WIFI_TX_BUF_LEN];

//------------------------------ GLOBAL DATA ----------------------------------
extern SemaphoreHandle_t osid_wifi_dma_smphr;
bluart_t g_uart_wifi;

//---------------------------- PUBLIC FUNCTIONS -------------------------------

bool bsp_wifi_init(void)
{
    bool is_ok;
    is_ok =  bsp_wifi_uart_init() && bsp_wifi_module_init();

    bsp_wifi_enable_rx_flags();

    return is_ok;
}

void bsp_wifi_turn_off(void)
{
    blgpio_dir(PIN_WIFI_PWR_EN, BLGPIO_DIR_OUT);
    blgpio_set(PIN_WIFI_PWR_EN, false);
}

void bsp_wifi_turn_on(void)
{
    blgpio_dir(PIN_WIFI_PWR_EN, BLGPIO_DIR_OUT);
    blgpio_set(PIN_WIFI_PWR_EN, true);
}

void bsp_wifi_enable_rx_flags(void)
{
    LL_USART_EnableDMAReq_RX(USART3);
    LL_USART_EnableIT_IDLE(USART3);
}
//--------------------------- PRIVATE FUNCTIONS -------------------------------
static bool bsp_wifi_module_init(void)
{
    blgpio_dir(PIN_WIFI_GPIO_0, BLGPIO_DIR_OUT);
    blgpio_set(PIN_WIFI_GPIO_0, true);

    blgpio_dir(PIN_WIFI_UART_CTS, BLGPIO_DIR_OUT);
    blgpio_set(PIN_WIFI_UART_CTS, false);

    return !bluart_configure(&g_uart_wifi, WIFI_DEFAULT_BAUD, 0);
}

static bool bsp_wifi_uart_init(void)
{
    bluart_error_t berr = BLUART_ERROR_OK;

    if (!berr)
    {
        berr = bluart_stm32_hal_init(&bluartstmhw0,
                                     PIN_WIFI_UART_TX, PIN_WIFI_UART_RX,
                                     -1, -1);
    }

    // overriding default start_read to do nothing
    memcpy(&wifi_uart_ops, bluartstmhw0.hw.ops,  sizeof(bluart_hw_ops_t));
    bluartstmhw0.hw.ops = &wifi_uart_ops;
    bluartstmhw0.hw.ops->start_read = bluart_stm32_hal_start_dma_read;

    if (!berr)
    {
        berr = bluart_init(&g_uart_wifi, &bluartstmhw0.hw, uart_wifi_buf,
                           UART_WIFI_RX_BUF_LEN, UART_WIFI_TX_BUF_LEN);
    }

    if (berr)
    {
        dprintf("WiFi UART init failed: %d\n", berr);
    }

    return !berr;

}

static bluart_error_t bluart_stm32_hal_start_dma_read (bluart_hw_t *hw)
{
    return BLUART_ERROR_OK;
}

//--------------------------- INTERRUPT HANDLERS ------------------------------

void bsp_wifi_USART3_IRQHandler(UART_HandleTypeDef *huart)
{
    uint32_t isrflags   = READ_REG(huart->Instance->ISR);
    uint32_t cr1its     = READ_REG(huart->Instance->CR1);
    uint32_t cr3its     = READ_REG(huart->Instance->CR3);
    uint32_t errorflags;

    // Next part is copied from HAL IRQ Handler to ensure bluart TX compatibility

    /* UART in mode Transmitter ------------------------------------------------*/
#if defined(USART_CR1_FIFOEN)
    if(((isrflags & USART_ISR_TXE_TXFNF) != RESET)
     && (   ((cr1its & USART_CR1_TXEIE_TXFNFIE) != RESET)
         || ((cr3its & USART_CR3_TXFTIE) != RESET)) )
#else
    if(((isrflags & USART_ISR_TXE) != RESET)
       && ((cr1its & USART_CR1_TXEIE) != RESET))
#endif
    {
        if (huart->TxISR != NULL) {huart->TxISR(huart);}
        return;
    }

    /* UART in mode Transmitter (transmission end) -----------------------------*/
    if(((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
    {
        /* Disable the UART Transmit Complete Interrupt */
        CLEAR_BIT(huart->Instance->CR1, USART_CR1_TCIE);

        /* Tx process is ended, restore huart->gState to Ready */
        huart->gState = HAL_UART_STATE_READY;

        /* Cleat TxISR function pointer */
        huart->TxISR = NULL;

        HAL_UART_TxCpltCallback(huart);
        return;
    }

    // Code for catching IDLE interrupt which HAL handler is missing

    /* Check for IDLE line interrupt */
    if (LL_USART_IsEnabledIT_IDLE(USART3) && LL_USART_IsActiveFlag_IDLE(USART3)) {
        LL_USART_ClearFlag_IDLE(USART3);        /* Clear IDLE line flag */

        xSemaphoreGiveFromISR(osid_wifi_dma_smphr, NULL);
    }
}