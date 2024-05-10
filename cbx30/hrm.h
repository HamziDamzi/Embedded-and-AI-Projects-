/** @file hrm.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef HRM_H
#define HRM_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------

#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <headers/ble_gap.h>
#include <components/ble/ble_services/ble_hrs_c/ble_hrs_c.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

#define RR_INTERVALS_MAX_CNT    (10u)

//----------------------------- DATA TYPES ------------------------------------

typedef struct hrm_message_
{
	int32_t timestamp;
	uint16_t milliseconds;
    uint16_t hr_value;
    uint16_t rr_intervals[RR_INTERVALS_MAX_CNT];
    uint8_t rr_intervals_cnt;
} hrm_message_t;

//----------------------------- STATIC DATA -----------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------

/**
 * Adds message queue to queue set
 * @param queue_set logger queue set
 */
void hrm_queue_to_set (QueueSetHandle_t queue_set);

/**
 * Gets queue handle for hrm message
 * @return queue handle
 */
QueueHandle_t hrm_get_queue_hndl(void);

/**
 * Returns maximum queue length
 * @return queue length
 */
uint16_t hrm_get_queue_len(void);

/**
 * Initialises hrm module
 * @return true on success, false otherwise
 */
bool hrm_init(void);

/**
 * Callback to be called when connection with hrm is established
 * @param p_name name of device
 * @param p_mac MAC address
 */
void hrm_connected_cb(uint8_t *p_name, uint8_t *p_mac);

/**
 * Callback to be called when connection with hrm is dropped
 * @param p_name name of device
 * @param p_mac MAC address
 */
void hrm_disconnected_cb(uint8_t *p_name, uint8_t *p_mac);

/**
 * Callback to be called when new data is received from hrm
 * @param p_hrm heart rate message
 * @return true on success, false otherwise
 */
bool hrm_data_cb(ble_hrm_t *p_hrm);

/**
 * Callback to be called when hrm device is scanned. Matching parameters are
 * compared to threshold values and on success device MAC is stored in memory
 * @param p_name device name
 * @param p_mac device MAC address
 * @param p_scan_param scan parameters
 * @param rssi signal strength
 */
void hrm_device_match_cb(uint8_t *p_name, ble_gap_addr_t *p_mac, 
	                     const ble_gap_scan_params_t *p_scan_param, int8_t rssi);

/**
 * Initiates pairing with hrm
 * @return true on success, false otherwise
 */
bool hrm_start_pair(void);

/**
 * Deletes device from memory and disconnects
 * @return true on success, false otherwise
 */
bool hrm_delete_pair(void);

/**
 * Initiates scanning
 * @return true on success, false otherwise
 */
bool hrm_start_scan(void);

/**
 * @brief Returns hrm_con_flag.
 * @return true -> connected
 * 		   false -> not connected
 */
bool hrm_get_conn (void);

void hrm_set_mac_filter(bool enabled);

#ifdef __cplusplus
}
#endif
#endif //HRM_H
