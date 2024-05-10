#ifndef _CROSSBOXFSMAUTO_HPP_
#define _CROSSBOXFSMAUTO_HPP_

#include <blib/FSM/FSM.hpp>
#include <blib/FSM/ControlData.hpp>
/*
Include .hpp file to your source file
A prototype for your own class, that you can copy-paste to your own header file
class crossboxFSM : public crossboxFSMAuto
{
    public:

    protected:
        bool callback(const Event& evt) override;
};
*/

class crossboxFSMAuto : public blib::FSM::FSM
{
    public:
        /* FSM-autogen: states */
        enum StateId : blib::FSM::StateId
        {
            Root = -1,
            Init,
            On,
            On_Idle,
            On_Charge,
            On_CheckMem,
            On_SessionActive,
            ChargeOff,
            Off,
        };
        static constexpr int num_states = 8;
        static constexpr int num_transitions = 30;
        /* FSM-autogen end */

        /* FSM-autogen: events */
        enum EventId : blib::FSM::EventId
        {
            enter = EnterEventId,
            leave = LeaveEventId,
            bleStart,
            bleStop,
            charged,
            chgOff,
            chgOn,
            chrgEvent,
            click,
            connected,
            disconnected,
            gpsEvt,
            initEvent,
            longPress,
            memFail,
            memOK,
            timeout,
        };
        /* FSM-autogen end */

        crossboxFSMAuto() : blib::FSM::FSM(control_data)
        {
            reset();
        }

    private:
        static const blib::FSM::ControlData<crossboxFSMAuto> control_data;

    protected:
        /* FSM-autogen: callbacks */
        virtual bool ChargedCb(const Event& evt) { return true; }
        virtual bool chargeEnter() { return true; }
        virtual void chargeExit() {}
        virtual bool chargeOffExit(const Event& evt) { return true; }
        virtual bool checkMemSize() { return true; }
        virtual bool gnssCheck(const Event& evt) { return true; }
        virtual bool OnCheck() { return true; }
        virtual bool OnConnected(const Event& evt) { return true; }
        virtual bool OnDisconnected(const Event& evt) { return true; }
        virtual bool powerOff() { return true; }
        virtual bool sessionStart() { return true; }
        virtual void sessionStop() {}
        virtual bool startUpDevice() { return true; }
        /* FSM-autogen end */
};

#endif
