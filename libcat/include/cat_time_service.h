#ifndef __CAT_TIME_SERVICE_H__
#define __CAT_TIME_SERVICE_H__

#include "cat_time_queue.h"

namespace cat {
class Kernel;
// ----------------------------------------------------------------------------
//! Time Service
class TimeService {
friend class cat::Kernel;
public:
    TimeService();
    ~TimeService();
    
    //! Get current time
    //! \return current time in milliseconds
    static Timestamp now();

    //! Post a timer to time service's queue
    //! The handler will be invoked from main thread upon tick time has reached
    //! \param handler Callback handler when timer expire
    //! \param message user-defined message to pass to handler
    //! \param tick How long for time timer, in milliseconds
    bool post_timer(TimerHandler<int>* handler, int message, Timestamp tick);
    //! Remove all timer assocated with the handler
    //! Any timer not fired yet will be discarded
    //! \param handler Callback handler when timer expire
    void remove_timer(TimerHandler<int>* handler);
    //! Remove timer assocated with the handler and with the message
    //! Any timer not fired yet will be discarded
    //! \param handler Callback handler when timer expire
    //! \param message user-defined message to pass to handler
    void remove_timer(TimerHandler<int>* handler, int message);

private:
    //! Called from kernel to process timer events
    bool timer();
    //! Called from kernel when the app is put to background
    void pause();
    //! Called from kernel when the app is resume to foreground
    void resume();

private:
    TimerQueue<int> m_timequeue;
    Timestamp m_last, m_tick;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_TIME_SERVICE_H__
