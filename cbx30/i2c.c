/** @file bsp_i2c.c
*
* @brief  i2c for STM32L4
*
* @par
* COPYRIGHT NOTICE: (c) 2016 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <inc/bsp/i2c.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stm32l4xx_hal.h>
#include <RTT.h>
#include <wdtm.h>

//-------------------------------- MACROS -------------------------------------
#define BSP_I2C_DEFAULT_TIMEOUT_MS  (10u)

#define DBGI2C(...)
//#define DBGI2C dprintf

//----------------------------- DATA TYPES ------------------------------------
typedef struct {
    // Handle for i2c instance.
    I2C_HandleTypeDef handle;
    // Used for locking instance during transfer (thread safe).
    SemaphoreHandle_t mutex_id;
    // Used for signaling end of interrupt
    SemaphoreHandle_t signal_id;
} i2c_master_t;

// Used for debugging purpose.
static int dbg_i2c_error;

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

/*!
* @brief Single transfer (Send/Receive) via i2c.
* @param[in] instance of i2c to transfer data with.
* @param[in] transfer structure containing mode, eventual mode attributes
*   (BSP_I2C_MODE_MEMORY mode applicable), device address (shifted one bit
*   to the left and OR-ed with I2C_READ_FLAG/I2C_WRITE_FLAG flag), pointer to
*   in/out data buffer, data length, transfer timeout in ms.
* @return 0 on success
*/
static int i2c_transfer_single(int instance, bsp_i2c_transfer_t *p_trx);

/*!
* @brief Clear eventual Notify/Signal. Called before starting transfer.
* @param[in] instance of i2c to clear Notify/Signal.
* @return 0 on success.
*/
static int i2c_signal_clear(int instance);

/*!
* @brief Wait for end of transfer Notify/Signal.
* @param[in] instance of i2c to wait for Notify/Signal.
* @param[in] timeout in ms to wait for Notify/Signal.
* @return 0 on success, 1 on timeout.
*/
static int i2c_signal_wait(int instance, uint8_t timeout);

/*!
* @brief Notify/Signal end of transfer. Called from I2C transfer complete ISR.
* @param[in] instance of i2c to send Notify/Signal.
* @return 0 on success.
*/
static int i2c_signal_send_isr(int instance);

/*!
* @brief Reset i2c instance. Automatically called on transfer failure.
*   transfer.
* @param[in] instance of i2c to reset.
* @return 0 on success.
*/
static int i2c_reset(int instance);

/*!
* @brief Lock instance so another thread cannot interrupt currently ongoing
*   transfer.
* @param[in] instance of i2c to lock.
* @return 0 on success.
*/
static int i2c_lock(int instance);

/*!
* @brief Unlock previously locked instance. Called on end of transfer.
* @param[in] instance of i2c to unlock.
* @return 0 on success.
*/
static int i2c_unlock(int instance);

/*!
* @brief Helper function, return number of allocated/on mcu available, i2c
*   master instances.
* @return number of allocated/available i2c master instances.
*/
static int i2c_instances_max(void);

/*!
* @brief Translate BSP_I2C_MODE_SEQUENCE_ to by HAL driver used sequence
*   value (I2C_XFEROPTIONS).
* @param[in] BSP_I2C_MODE_SEQUENTIAL_ value.
* @return I2C_XFEROPTIONS numerical value.
*/
static int i2c_sequence_translate(bsp_i2c_mode_t sequence);

/*!
* @brief Assert function prints file and line where error is detected. Called on
*   setup failure (wrong parameters).
* @param[in] __FILE__
* @param[in] __LINE__
* @return none
*/
static void bsp_i2c_assert(const char *p_file, int line);

/*!
* @brief CubeMX generated i2c initialization code for i2c1 instance.
* @return none
*/
static void hal_i2c1_mx_init(i2c_master_t *master);

/*!
* @brief CubeMX generated i2c initialization code for i2c2 instance.
* @return none
*/
static void hal_i2c2_mx_init(i2c_master_t *master);

/*!
* @brief CubeMX generated i2c initialization code for i2c3 instance.
* @return none
*/
static void hal_i2c3_mx_init(i2c_master_t *master);

//----------------------- STATIC DATA & CONSTANTS -----------------------------

// I2C master for every i2c peripheral on STM32L476RG device.
static i2c_master_t i2c_master[3u];

// Flag set in bsp_i2c_transfer function if scheduler is started to decide what
// kind of transfer to use.
static int rtos_mode_flag;

static uint8_t b_i2c_init_flag = 0;
//------------------------------ GLOBAL DATA ----------------------------------

//---------------------------- PUBLIC FUNCTIONS -------------------------------
uint8_t bsp_i2c_get_init_flag()
{
    return b_i2c_init_flag;
}

void bsp_i2c_init_nortos(void)
{
    // CubeMX init inside.
    hal_i2c1_mx_init(&i2c_master[0]);
    hal_i2c2_mx_init(&i2c_master[1]);
    hal_i2c3_mx_init(&i2c_master[2]);
}

int bsp_i2c_init (void)
{
    static int result = 1u;

    if (result)
    {
        result = 0;

        // CubeMX init inside.
        hal_i2c1_mx_init(&i2c_master[0]);
        hal_i2c2_mx_init(&i2c_master[1]);
        hal_i2c3_mx_init(&i2c_master[2]);

        // Create mutexes and signals.
        for (int index = 0; index < i2c_instances_max(); index++)
        {
            i2c_master[index].mutex_id = xSemaphoreCreateMutex();
            result = result || (NULL == i2c_master[index].mutex_id);
            i2c_master[index].signal_id = xSemaphoreCreateBinary();
            result = result || (NULL == i2c_master[index].signal_id);
        }

        // Clear mutexes and signals on failure.
        for (int index = 0; ((index < i2c_instances_max()) && (result));
             index++)
        {
            if (NULL != i2c_master[index].mutex_id)
            {
                vQueueDelete(i2c_master[index].mutex_id);
                i2c_master[index].mutex_id = NULL;
            }

            if (NULL != i2c_master[index].signal_id)
            {
                vQueueDelete(i2c_master[index].signal_id);
                i2c_master[index].signal_id = NULL;
            }

        }
    }

    if (result)
    {
        DBGI2C("\nbsp_i2c_init failed.");
    }

    b_i2c_init_flag = !result;

    return result;
}


int bsp_i2c_transfer
    (int instance, bsp_i2c_transfer_t *p_trx, int trx_len)
{
    int result = !((0 < trx_len) &&
                (NULL != p_trx) &&
                (0 < instance) &&
                (i2c_instances_max() > (instance - 1)));

    rtos_mode_flag = (taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState());

    if (!result)
    {
        // Normalize instance (index).
        instance = instance - 1;

        i2c_lock(instance);

        for (int index = 0; ((index < trx_len) && (0 == result)); index++)
        {
            if (NULL != p_trx[index].prepare)
            {
                p_trx[index].prepare(p_trx);
            }

            result = i2c_transfer_single(instance, &p_trx[index]);
        }

        i2c_unlock(instance);
    }

    if (result)
    {
        DBGI2C("\nbsp_i2c_transfer failed.");
    }

    if (dbg_i2c_error)
    {
        DBGI2C("\ni2c_error: %d.", dbg_i2c_error);
    }

    return result;
}


void *bsp_i2c_handle_get(int instance)
{
    void *p_result = NULL;
    if ((0 < instance) && (i2c_instances_max() > (instance - 1)))
    {
        instance = instance - 1;
        p_result = &i2c_master[instance].handle;
    }

    return p_result;
}


void bsp_i2c_reset(int instance)
{
    if ((0 < instance) && (i2c_instances_max() > (instance - 1)))
    {
        i2c_reset(instance);
    }
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

static int i2c_transfer_single(int instance, bsp_i2c_transfer_t *p_trx)
{
    I2C_HandleTypeDef *p_handle = &i2c_master[instance].handle;
    int result = 0;

    i2c_signal_clear(instance);

    uint8_t timeout = p_trx->timeout;
    if (0 == timeout)
    {
        timeout = BSP_I2C_DEFAULT_TIMEOUT_MS;
    }


    if (BSP_I2C_READ_FLAG & p_trx->address)
    {
        uint8_t address = (p_trx->address & (~BSP_I2C_READ_FLAG));
        if (rtos_mode_flag)
        {
            if (BSP_I2C_MODE_NORMAL == p_trx->mode)
            {
                result = (HAL_OK != HAL_I2C_Master_Receive_IT(p_handle,
                                                              address,
                                                              p_trx->data,
                                                              p_trx->length));
            }
            else if (BSP_I2C_MODE_MEMORY == p_trx->mode)
            {
                result = (HAL_OK != HAL_I2C_Mem_Read_IT(p_handle, address,
                                                        p_trx->mem_addr,
                                                        p_trx->mem_addr_len,
                                                        p_trx->data,
                                                        p_trx->length));

            }
            else if ((BSP_I2C_MODE_SEQUENTIAL_FIRST_FRAME <= p_trx->mode) &&
                     (BSP_I2C_MODE_SEQUENTIAL_LAST_FRAME >= p_trx->mode))
            {
                uint32_t sequence = i2c_sequence_translate(p_trx->mode);
                result = (HAL_OK !=
                          HAL_I2C_Master_Sequential_Receive_IT(p_handle,
                                                               address,
                                                               p_trx->data,
                                                               p_trx->length,
                                                               sequence));
            }
            else
            {
                bsp_i2c_assert(__FILE__, __LINE__);
            }
        }
        else
        {
            if (BSP_I2C_MODE_NORMAL == p_trx->mode)
            {
                result = (HAL_OK != HAL_I2C_Master_Receive(p_handle,
                                                              address,
                                                              p_trx->data,
                                                              p_trx->length,
                                                              timeout));
            }
            else if (BSP_I2C_MODE_MEMORY == p_trx->mode)
            {
                result = (HAL_OK != HAL_I2C_Mem_Read(p_handle, address,
                                                        p_trx->mem_addr,
                                                        p_trx->mem_addr_len,
                                                        p_trx->data,
                                                        p_trx->length,
                                                        timeout));
            }
            else
            {
                bsp_i2c_assert(__FILE__, __LINE__);
            }
        }
    }
    else
    {
        uint8_t address = p_trx->address;
        if (rtos_mode_flag)
        {
            if (BSP_I2C_MODE_NORMAL == p_trx->mode)
            {
                result = (HAL_OK != HAL_I2C_Master_Transmit_IT(p_handle,
                                                               address,
                                                               p_trx->data,
                                                               p_trx->length));
            }
            else if (BSP_I2C_MODE_MEMORY == p_trx->mode)
            {
                result = (HAL_OK != HAL_I2C_Mem_Write_IT(p_handle, address,
                                                         p_trx->mem_addr,
                                                         p_trx->mem_addr_len,
                                                         p_trx->data,
                                                         p_trx->length));
            }
            else if ((BSP_I2C_MODE_SEQUENTIAL_FIRST_FRAME <= p_trx->mode) &&
                     (BSP_I2C_MODE_SEQUENTIAL_LAST_FRAME >= p_trx->mode))
            {
                uint32_t sequence = i2c_sequence_translate(p_trx->mode);
                result = (HAL_OK !=
                          HAL_I2C_Master_Sequential_Transmit_IT(p_handle,
                                                                address,
                                                                p_trx->data,
                                                                p_trx->length,
                                                                sequence));
            }
            else
            {
                bsp_i2c_assert(__FILE__, __LINE__);
            }
        }
        else
        {
            if (BSP_I2C_MODE_NORMAL == p_trx->mode)
            {
                result = (HAL_OK != HAL_I2C_Master_Transmit(p_handle,
                                                            address,
                                                            p_trx->data,
                                                            p_trx->length,
                                                            timeout));
            }
            else if (BSP_I2C_MODE_MEMORY == p_trx->mode)
            {
                result = (HAL_OK != HAL_I2C_Mem_Write(p_handle, address,
                                                      p_trx->mem_addr,
                                                      p_trx->mem_addr_len,
                                                      p_trx->data,
                                                      p_trx->length,
                                                      timeout));
            }
            else
            {
                bsp_i2c_assert(__FILE__, __LINE__);
            }
        }
    }

    if (!result)
    {
        result = i2c_signal_wait(instance, timeout);
    }

    if (result)
    {
        DBGI2C("\nbsp_i2c_transfer_single failed.");
        i2c_reset(instance);
    }


    return result;
}


static int i2c_signal_clear(int instance)
{
    if (rtos_mode_flag)
    {
        xSemaphoreTake(i2c_master[instance].signal_id, 0);
    }

    return 0;
}


static int i2c_signal_wait(int instance, uint8_t timeout)
{
    int result = 0;

    if (rtos_mode_flag)
    {
        TickType_t ticks = timeout / portTICK_PERIOD_MS;

        if (pdTRUE == xSemaphoreTake(i2c_master[instance].signal_id, ticks))
        {
            xSemaphoreGive(i2c_master[instance].signal_id);
        }
        else
        {
            DBGI2C("\ni2c_signal_wait failed (%d).", dbg_signal_wait);
            result = 1;
        }
    }

    return result;
}


static int i2c_signal_send_isr (int instance)
{
    int result = 0;

    if (rtos_mode_flag)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        result = (pdPASS !=
                  xSemaphoreGiveFromISR(i2c_master[instance].signal_id,
                                        &xHigherPriorityTaskWoken));
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    return result;
}


static int i2c_reset (int instance)
{
    I2C_HandleTypeDef *p_handle = &i2c_master[instance].handle;

    if (p_handle)
    {
        dprint("X1-");
        __HAL_I2C_DISABLE(p_handle);
        dprint("X2-");
        DBGI2C("\ni2c reset");
        dprint("X3-");
        // Reinit i2c instance here if simply reset is not enough.
        p_handle->State = (volatile HAL_I2C_StateTypeDef)HAL_I2C_STATE_READY;
        dprint("X4-");
        __HAL_I2C_ENABLE(p_handle);
        dprint("X5-");
    }

    return 0;
}


static int i2c_lock (int instance)
{
    int result = 0;

    if (rtos_mode_flag)
    {
        SemaphoreHandle_t mutex_id = i2c_master[instance].mutex_id;
        int result = (pdTRUE != xSemaphoreTake(mutex_id, portMAX_DELAY));

        if (result)
        {
            DBGI2C("\ni2c_lock failed.");
        }
    }

    return result;
}


static int i2c_unlock (int instance)
{
    int result = 0;

    if (rtos_mode_flag)
    {
        SemaphoreHandle_t mutex_id = i2c_master[instance].mutex_id;
        result = (pdTRUE != xSemaphoreGive(mutex_id));

        if (result)
        {
            DBGI2C("\ni2c_unlock failed.");
        }
    }

    return result;
}


static int i2c_instances_max(void)
{
    return (sizeof(i2c_master)/sizeof(i2c_master[0]));
}


static int i2c_sequence_translate(bsp_i2c_mode_t sequence)
{
    int result = 0;
    sequence = sequence - BSP_I2C_MODE_SEQUENTIAL_FIRST_FRAME;
    
    const int hal_sequence[] = {
        I2C_FIRST_FRAME, 
        I2C_FIRST_AND_NEXT_FRAME, 
        I2C_NEXT_FRAME, 
        I2C_FIRST_AND_LAST_FRAME, 
        I2C_LAST_FRAME
    };
    
    if ((sizeof(hal_sequence)/sizeof(hal_sequence[0])) > sequence)
    {
        result = hal_sequence[sequence];
    }
    else
    {
        bsp_i2c_assert(__FILE__, __LINE__);
    }

    return result;
}


static void bsp_i2c_assert(const char *p_file, int line)
{
    dprintf("\nI2C setup error in file %s, line %d.", p_file, line);
    for (;;);
}

//---------------------------- PROJECT SPECIFIC -------------------------------

static void hal_i2c_init_error(void *p_instance)
{
    (void)p_instance;
    for(;;);
}

// CubeMX i2c init for pmic.
static void hal_i2c1_mx_init(i2c_master_t *master)
{
    I2C_HandleTypeDef *p_handle = &master->handle;

    p_handle->Instance = I2C1;
    p_handle->Init.Timing = 0x103089ff; // Timing constant for 100kbps @ 24MHz PCLK1;
    p_handle->Init.OwnAddress1 = 0;
    p_handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    p_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    p_handle->Init.OwnAddress2 = 0;
    p_handle->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    p_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    p_handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(p_handle) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }

    /**Configure Analogue filter
    */
    if (HAL_I2CEx_ConfigAnalogFilter(p_handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }

    /**Configure Digital filter
    */
    if (HAL_I2CEx_ConfigDigitalFilter(p_handle, 0) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }
}


// CubeMX i2c init for FDC.
static void hal_i2c2_mx_init(i2c_master_t *master)
{
    I2C_HandleTypeDef *p_handle = &master->handle;

    p_handle->Instance = I2C2;
    // Timing constant 0x00506682 for 100kbps @ 24MHz PCLK1;
    // Timing constant 0x00506682 for 100kbps @ 48MHz PCLK1;
    p_handle->Init.Timing = 0x103089ff;
    p_handle->Init.OwnAddress1 = 0;
    p_handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    p_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    p_handle->Init.OwnAddress2 = 0;
    p_handle->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    p_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    p_handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(p_handle) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }

    /**Configure Analogue filter
    */
    if (HAL_I2CEx_ConfigAnalogFilter(p_handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }

    /**Configure Digital filter
    */
    if (HAL_I2CEx_ConfigDigitalFilter(p_handle, 0) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }
}

// CubeMX i2c init for ACC.
static void hal_i2c3_mx_init(i2c_master_t *master)
{
    I2C_HandleTypeDef *p_handle = &master->handle;

    p_handle->Instance = I2C3;

    // 0x00506682; Timing constant for 100kbps @ 24MHz PCLK1;
    // 0x00200b3d; Timing constant for 300kbps @ 24MHz PCLK1;
    // 0x00200c28; Timing constant for 400kbps @ 24MHz PCLK1;
    // 0x00702991; Timing constant for 400kbps @ 48MHz PCLK1;
    p_handle->Init.Timing = 0x00702991;
    p_handle->Init.OwnAddress1 = 0x33;
    p_handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    p_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    p_handle->Init.OwnAddress2 = 0;
    p_handle->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    p_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    p_handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(p_handle) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }

    /**Configure Analogue filter
    */
    if (HAL_I2CEx_ConfigAnalogFilter(p_handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }

    /**Configure Digital filter
    */
    if (HAL_I2CEx_ConfigDigitalFilter(p_handle, 0) != HAL_OK)
    {
        hal_i2c_init_error(p_handle->Instance);
    }
}


static void i2c_transfer_complete (I2C_HandleTypeDef *hi2c)
{
    int max = i2c_instances_max();

    for (int index = 0; index < max; index++)
    {
        if (i2c_master[index].handle.Instance == hi2c->Instance)
        {
            i2c_signal_send_isr(index);
            index = max;
        }
    }
}

//--------------------------- INTERRUPT HANDLERS ------------------------------
/**
 * @brief ISR callback for handling end of I2C receive
 * @return void
 */
void HAL_I2C_MasterRxCpltCallback (I2C_HandleTypeDef *hi2c)
{
    i2c_transfer_complete(hi2c);
}


/**
 * @brief ISR callback for handling end of I2C transmit
 * @return void
 */
void HAL_I2C_MasterTxCpltCallback (I2C_HandleTypeDef *hi2c)
{
    i2c_transfer_complete(hi2c);
}

/**
 * @brief ISR callback for handling end of I2C mem receive
 * @return void
 */
void HAL_I2C_MemRxCpltCallback (I2C_HandleTypeDef *hi2c)
{
    i2c_transfer_complete(hi2c);
}


/**
 * @brief ISR callback for handling end of I2C mem transmit
 * @return void
 */
void HAL_I2C_MemTxCpltCallback (I2C_HandleTypeDef *hi2c)
{
    i2c_transfer_complete(hi2c);
}


void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    dbg_i2c_error++;
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
    dbg_i2c_error++;
}
