/** @file helpers.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_HELPERS_H
#define CROSSBOX_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif
//------------------------------ INCLUDES -------------------------------------

//-------------------------- CONSTANTS & MACROS -------------------------------

#define countof(__array) sizeof(__array)/sizeof(__array[0])

//----------------------------- DATA TYPES ------------------------------------

//----------------------------- STATIC DATA -----------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Sort array of uint32_t types.
 * @param array to sort
 * @param size of array
 * @param direction; true - low2high, false - high2low
 */
void array_u32_sort(uint32_t * array, uint32_t size, char low2high);

/**
 * @brief Sort array of uint16_t types.
 * @param array to sort
 * @param size of array
 * @param direction; true - low2high, false - high2low
 */
void array_u16_sort(uint16_t * array, uint32_t size, char low2high);

/**
 * @brief Sort array of uint8_t types.
 * @param array to sort
 * @param size of array
 * @param direction; true - low2high, false - high2low
 */
void array_u8_sort(uint8_t * array, uint32_t size, char low2high);

/**
 * @brief Sort array of int32_t types.
 * @param array to sort
 * @param size of array
 * @param direction; true - low2high, false - high2low
 */
void array_i32_sort(int32_t * array, uint32_t size, char low2high);

/**
 * @brief Sort array of int16_t types.
 * @param array to sort
 * @param size of array
 * @param direction; true - low2high, false - high2low
 */
void array_i16_sort(int16_t * array, uint32_t size, char low2high);

/**
 * @brief Sort array of int8_t types.
 * @param array to sort
 * @param size of array
 * @param direction; true - low2high, false - high2low
 */
void array_i8_sort(int8_t * array, uint32_t size, char low2high);

/**
 * @brief This functions returns absolute value of int32_t.
 * @param num input value.
 * @return Absolute value of input.
 */
uint32_t abs32(int32_t num);

/**
 * @brief This functions returns absolute value of int16_t.
 * @param num input value.
 * @return Absolute value of input.
 */
uint16_t abs16(int16_t num);

/**
 * @brief This functions returns absolute value of int8_t.
 * @param num input value.
 * @return Absolute value of input.
 */
uint8_t abs8(int8_t num);

/**
 * @brief This function returns checksum of array, with initial value.
 * @param ics initial value.
 * @param data array to calculate checksum.
 * @param length of array.
 * @return uint16_t checksum.
 */
uint16_t cs16i(uint16_t ics, const uint8_t * data, uint16_t length);

/**
 * @brief This function returns checksum of array.
 * @param ics offset.
 * @param data array to calculate checksum.
 * @param length of array.
 * @return checksum value.
 */
uint16_t cs16(const uint8_t * data, uint16_t length);

/**
 * @brief Cyclic redundancy check, with initial value.
 * @param icrc initial value.
 * @param p_data array to calculate CRC.
 * @param length of array.
 * @return CRC value.
 */
uint16_t crc16i(uint16_t icrc, const char * p_data, uint16_t length);

/**
 * @brief Cyclic redundancy check.
 * @param p_data array to calculate CRC.
 * @param length of array.
 * @return CRC value.
 */
uint16_t crc16(const uint8_t * data, uint16_t length);


/**
 * @brief Convert array of "hexadecimal" characters to uint32_t number.
 * @param p_str array to convert.
 * @param length of array.
 * @return decimal value of input.
 */
uint32_t hex2int(const char *p_str, char len);

/**
 * @brief Reverse bytes in array (highendian <-> lowendian).
 * @param p_data pointer to first member of array.
 * @param len number of items in array.
 */
void reverse_bytes(uint8_t *p_data, uint16_t len);

/**
 * @brief Convert "hexadecimal" character to number.
 * @param chr charcter to convert (0 - 1, A - F, a - f).
 * @return uint8_t in range 0 - 15 if argument is valid, '?' otherwise.
 */
uint8_t char2nibble(uint8_t chr);

/**
 * @brief Convert nibble to "hexadecimal" character
 * @param nib nibble to convert (range 0x0 - 0xF)
 * @return uint8_t character ('0' - '9', 'A' - 'F') if argument is valid,
 *         '?' otherwise
 */
uint8_t nibble2char(uint8_t nib);

#ifdef __cplusplus
}
#endif
#endif //CROSSBOX_HELPERS_H
