/** @file imu.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_IMU_H
#define CROSSBOX_IMU_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Initialize IMU SPI.
 * @return 0 -> ok
 *         not 0 -> fail
 */
uint8_t bsp_imu_spi_init ();

/**
 * @brief Read from IMU registers.
 * @param reg_addr : Address of the register we want to read from.
 * @param p_buff : Pointer to array where we want to store register data.
 * @param buff_len : Length of the array.
 * @return 0 -> ok
 *         not 0 -> fail
 */
uint8_t bsp_imu_spi_read (uint8_t reg_addr, uint8_t *p_buff, uint16_t buff_len);

/**
 * @brief Write to IMU registers.
 * @param reg_addr : Address of the register we want to write to.
 * @param p_buff : Pointer to data we want to write to register.
 * @param buff_len : Length of the data.
 * @return 0 -> ok
 *         not 0 -> fail
 */
uint8_t bsp_imu_spi_write (uint8_t reg_addr, uint8_t *p_buff, uint16_t buff_len);


#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_IMU_H

