/** @file fsm_helper.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_FSM_HELPER_H
#define CROSSBOX_FSM_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>
#include <stdbool.h>


//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief Return state of session.
 * @return true -> session is active.
 *         false -> session is not active.
 */
bool fsm_helper_is_active (void);

/**
 * @brief Execute delayed fsm event
 * @return true -> session is active.
 *         false -> session is not active.
 */
bool fsm_helper_gen_memfail_evt_with_delay(uint32_t delay_ms);

/**
 * @brief Send charger on event to Crossbox FSM.
 */
void fsm_helper_chg_on (void);

/**
 * @brief Send charger off event to Crossbox FSM.
 */
void fsm_helper_chg_off (void);

/**
 * @brief Send charged event to Crossbox FSM.
 */
void fsm_helper_charged (void);

/**
 * @brief Send GNSS state change event to Crossbox FSM.
 */
void fsm_helper_gnss_evt (void);

/**
 * @brief Send BLE connected event to Crossbox FSM
 */
void fsm_helper_ble_connected_evt (void);

/**
 * @brief Send BLE disconnected event to Crossbox FSM
 */
void fsm_helper_ble_disconnected_evt (void);

/**
 * @brief Send BLE start event.
 */
void fsm_helper_ble_start_evt (void);

/**
 * @brief Send BLE stop event.
 */
void fsm_helper_ble_stop_evt (void);

/**
 * @brief Send timeout event.
 */
void fsm_helper_power_off (void);

#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_FSM_HELPER_H

