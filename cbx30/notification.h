/** @file notification.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/


#ifndef CROSSBOX_NOTIFICATION_H
#define CROSSBOX_NOTIFICATION_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

typedef enum _notification_t{
    NOTIFY_DEVICE_ON,
    NOTIFY_DEVICE_OFF,
    NOTIFY_SESSION_START,
    NOTIFY_SESSION_PAUSE,
    NOTIFY_SESSION_STOP,
    NOTIFY_GNSS_START,
    NOTIFY_GPS_LOCK_ACQUIRED,
    NOTIFY_GPS_LOCK_LOST,
    NOTIFY_BLE_CONNECT,
    NOTIFY_BLE_DISCONNECT,
    NOTIFY_CHARGE_CONNECT,
    NOTIFY_CHARGE_COMPLETE,
    NOTIFY_NO_MEMORY
} notification_t;

//----------------------------- STATIC DATA -----------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------

/**
 *
 * Plays signals based on event (turns on LED and/or triggers buzzer)
 * @param type of event that occured
 */
void notification_play(notification_t event);

#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_NOTIFICATION_H
