/** @file crossbox_fsm.hpp
 *
 * @brief See source file.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
 * All rights reserved.
 */

#ifndef CROSSBOX_CROSSBOX_FSM_HPP
#define CROSSBOX_CROSSBOX_FSM_HPP

#include <crossboxFSMAuto.hpp>

using namespace blib::FSM;

/*FSM crossbox:
 *    -> Init:
 *        @initEvent -> On
 *        @chrgEvent -> ChargeOff
 * 
 *  ChargeOff:
 *      @enter chargeEnter
 *      @charged ChargedCb
 *      @chgOff -> Off
 *      @longPress chargeOffExit -> On
 * 
 *  On:
 *      @enter startUpDevice -> >::Idle
 *      @timeout -> Off
 *      @longPress -> Off
 *      @connected OnConnected
 *      @disconnected OnDisconnected
 *
 *  On::Idle:
 *      @enter OnCheck
 *      @gpsEvt gnssCheck
 *      @chgOn -> ::Charge
 *      @click -> ::CheckMem
 *      @bleStart -> ::CheckMem
 *
 *  On::CheckMem:
 *      @enter checkMemSize
 *      @memFail -> ::Idle
 *      @memOK -> ::SessionActive
 *
 *  On::Charge:
 *      @enter chargeEnter
 *      @chgOff -> ::Idle
 *      @charged ChargedCb
 *      @leave chargeExit
 *      @click -> ::CheckMem
 *      @bleStart -> ::CheckMem
 * 
 * 
 *  On::SessionActive:
 *      @enter sessionStart
 *      @click -> ::Idle
 *      @bleStop -> ::Idle
 *      @leave sessionStop
 *
 *  Off:
 *      @enter powerOff
 */
/**
 * @brief This function initialize/start finite state machine and belonging
 *        button task.
 * @return True if task is started successfully, false otherwise.
 */
bool crossbox_fsm_init();

class crossboxFSM : public crossboxFSMAuto
{
protected:
    bool powerOff() override;
    bool sessionStart() override;
    bool startUpDevice() override;
    void sessionStop() override;
    bool OnConnected(const Event& evt) override;
    bool OnDisconnected(const Event& evt) override;
    bool chargeEnter() override;
    bool OnCheck() override;
    bool ChargedCb(const Event& evt) override;
    void chargeExit() override;
    bool chargeOffExit(const Event& evt);
    bool gnssCheck(const Event& evt) override;
    bool checkMemSize() override;
};

/**
 * @brief Return state of session.
 * @return true -> session is active.
 *         false -> session is not active.
 */
bool is_active_state ();

/**
 * @brief Send charger on event to Crossbox FSM.
 */
void crossboxfsm_send_chg_on (void);

/**
 * @brief Send charger off event to Crossbox FSM.
 */
void crossboxfsm_send_chg_off (void);

/**
 * @brief Send charged event to Crossbox FSM.
 */
void crossboxfsm_send_charged (void);

/**
 * @brief Send GNSS state change event to Crossbox FSM.
 */
void crossboxfsm_send_gnss_evt (void);

/**
 * @brief Send connection established event to Crossbox FSM.
 */
void crossboxfsm_send_ble_connected (void);

/**
 * @brief Send disconnect event to Crossbox FSM.
 */
void crossboxfsm_send_ble_disconnected (void);

/**
 * @brief Send arb. event to Crossbox FSM.
 */
void crossboxfsm_send_memfail_event(Event event);

/**
 * @brief Send bleStart event to Crossbox FSM.
 */
void crossboxfsm_bleStart(void);

/**
 * @brief Send bleStop event to Crossbox FSM.
 */
void crossboxfsm_bleStop(void);

/**
 * @brief Send low battery event.
 */
void crossboxfsm_lowBat(void);

/**
 * @brief Sends timeout event to fsm which powers off device
 */
void crossboxfsm_powerOff(void);

/**
 * @brief Leave init function.
 */
void crossboxfsm_leaveInit(void);


#endif //CROSSBOX_CROSSBOX_FSM_HPP
