/** @file ble_Service.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/


#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <ble_gap.h>
//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//----------------------------- STATIC DATA -----------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------

/**@brief Function for setting configuration for the BLE stack.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_services_cfg_set(uint8_t conn_cfg_tag);

/**@brief Function for initializing the BLE stack.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_services_stack_init(void);

/**@brief Function for setting the advertisement data.
 * @details Sets the full device name and its available BLE services in the advertisement data.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_services_advertisement_data_set (void);

/**
 * @brief Starts ble advertisement.
 */
void ble_services_nus_adv_start(void);

/**
 * @brief Inits nus service (Ble stack, gap, gatt...).
 * @return Must return 0.
 */
int ble_services_nus_init(void);

/**
 * @brief Sends nus data.
 * @param p_snd_data Tx data.
 * @param len Len of tx data.
 */
void ble_services_nus_send(uint8_t *p_snd_data, uint32_t len);


/**
 * Initialises scan parameters
 */
void ble_services_scan_init(void);

/**
 * Starts scanning for devices
 */
void ble_services_scan_start(void);

/**
 * Connects to scanned HRM service
 * @param p_mac address of HRM device
 * @param p_scan_param scan parameters
 * @return true on success, false otherwise
 */
bool ble_services_connect(ble_gap_addr_t *p_mac, const ble_gap_scan_params_t *p_scan_param);

/**
 * Disconnects from HRM device
 */
void ble_services_hrm_disconnect(void);

#ifdef __cplusplus
}
#endif

#endif
