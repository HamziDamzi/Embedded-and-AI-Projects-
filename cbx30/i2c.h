/** @file bsp_i2c.h
*
* @brief i2c for STM32L4
*
* @par
* COPYRIGHT NOTICE: (c) 2016 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>

//-------------------------- CONSTANTS & MACROS -------------------------------
#define BSP_I2C_READ_FLAG      (0x01)
#define BSP_I2C_WRITE_FLAG     (0x00)


//----------------------------- DATA TYPES ------------------------------------
typedef enum {
    BSP_I2C_MODE_NORMAL = 0,                        // Normal transfer mode.
    BSP_I2C_MODE_MEMORY,                            // Device address following one or two bytes register/location address, following data.
    BSP_I2C_MODE_SEQUENTIAL_FIRST_FRAME,            // Generate START condition, without stop. Use when there is direction change in next frame. 
    BSP_I2C_MODE_SEQUENTIAL_FIRST_AND_NEXT_FRAME,   // Generate START condition, without stop. Use when there is NO direction change in next frame.
    BSP_I2C_MODE_SEQUENTIAL_NEXT_FRAME,             // No START nor STOP, simply continue transfer.
    BSP_I2C_MODE_SEQUENTIAL_FIRST_AND_LAST_FRAME,   // Generate START and STOP, equal to normal transfer mode.
    BSP_I2C_MODE_SEQUENTIAL_LAST_FRAME,             // Generate STOP at end of frame.
} bsp_i2c_mode_t;

typedef struct {
    uint8_t *data;                                  // Pointer to tx/rx data buffer.
    void (*prepare)(void*);                         // Pointer to eventual prepare function (called before transfer).
    uint16_t length;                                // Number of bytes to transfer.
    uint16_t mem_addr;                              // Register/Memory address. Used in BSP_I2C_MODE_MEMORY mode only.
    uint8_t mem_addr_len;                           // Size of Register/Memory address, 1 or 2. Used in BSP_I2C_MODE_MEMORY mode only.
    uint8_t mode;                                   // Transfer mode (bsp_i2c_mode_t)
    uint8_t address;                                // Device address (shifted one bit to the left and OR-ec with I2C_READ_FLAG/I2C_WRITE_FLAG.
    uint8_t timeout;                                // Time to wait for end of transfer. BSP_I2C_DEFAULT_TIMEOUT_MS value used if set to 0.
} bsp_i2c_transfer_t;


//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Returns I2C init flag.
 * @return 1 -> initialize.
 *         0 -> not initialize.
 */
uint8_t bsp_i2c_get_init_flag();

/*!
* @brief Init required i2c instances, without create locking mutexes and assign
*       signals.
* @return void
*/
void bsp_i2c_init_nortos(void);

/*!
* @brief Init required i2c instances, create locking mutexes and assign
*       signals.
* @return 0 on success
*/
int bsp_i2c_init(void);


/*!
* @brief Transfer (Send/Receive) via i2c.
* @param[in] instance of i2c to transfer data with (1, 2 or 3).
* @param[in] pointer to transfer structure(s) containing mode, eventual mode
*   attributes (BSP_I2C_MODE_MEMORY mode applicable), device address (shifted
*   one bit to the left and OR-ed with I2C_READ_FLAG/I2C_WRITE_FLAG flag),
*   pointer to in/out data buffer, data length, transfer timeout in ms.
* @param[in] number of transfer structures to process.
* @return 0 on success
*/
int bsp_i2c_transfer
    (int instance, bsp_i2c_transfer_t *p_trx, int trx_len);


/*!
* @brief Return pointer to i2c handle. Used for accesing handle from i2c irq
* @param[in] instance of i2c (1, 2 or 3).
* @return pointer to handle on success, NULL otherwise.
*/
void *bsp_i2c_handle_get(int instance);


/*!
* @brief Reset i2c instance
* @param[in] instance of i2c (1, 2 or 3).
* @return none.
*/
void bsp_i2c_reset(int instance);

#ifdef __cplusplus
}
#endif
#endif //__BSP_I2C_H__
