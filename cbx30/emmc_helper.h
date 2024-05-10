/** @file emmc_helper.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_EMMC_HELPER_H
#define CROSSBOX_EMMC_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>
#include <stdio.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

#define EMMC_DATABUFF               (256u)
#define EMMC_DATA_BUFFER_SIZE       (8u * 1024)
#define EMMC_INDEX_BUFFER_SIZE      (1u * 1024)
#define INDEX_LENGTH                (6u)
#define SESSION_FILENAME_LEN        (30u)
#define ID_FILENAME_LEN             (30u)

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
* @brief Format file system.
* @return 1 on success, 0 otherwise.
*/
char filesystem_format(void);

/**
* @brief Mount file system.
* @return 1 on success, 0 otherwise.
*/
char filesystem_mount(void);

/**
 * @brief Writes live session data from queue to data buffer
 * @param p_data Data sent from queue
 * @param p_buff Databuff from queue gets in buffer
 * @param len Length of data from databuffer
 */
void emmc_data_write_buffer(uint8_t *p_data, uint8_t *p_buff, uint16_t len);

/**
 * @brief Write live session index to index buffer
 * @param len Lenth of data from databuffer
 * @param p_buff Index buffer
 */
void emmc_index_write_buffer(uint16_t len, uint8_t *p_buff);

/**
 * @brief Write from Data buffer to data file
 * @param p_filename File for writting from data buffer to data file
 * @param p_buff Data buffer
 */
bool emmc_write_data_file(char *p_filename, uint8_t *p_buff);

/**
 * @brief Write from Index buffer to index file
 * @param p_filename File for writting from index buffer to index file
 * @param p_buff Index buffer
 */
bool emmc_write_index_file(char *p_filename, uint8_t *p_buff);

/**
 * @brief check if data buffer is almost full
 * @return 1 - succes, 0 - fail
 */
bool emmc_is_buffer_full(void);

/**
 * @brief closes data file
 * @return 0 - sucess, 1 fail
 */
uint8_t emmc_helper_close_data(void);


/**
 * @brief closes index file
 * @return 0 - sucess, 1 fail
 */
uint8_t emmc_helper_close_index(void);

/**
 * @brief Rename given file to new file name.
 * 
 * @param p_old_filename Old file name
 * @param p_new_filename New file name
 * @return 0 - sucess, 1 fail
 */
uint8_t emmc_helper_rename_file(char *p_old_filename, char *p_new_filename);

#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_EMMC_HELPER_H

