#include "osal_time_service.h"
#include <chrono>

using namespace osal;

// ----------------------------------------------------------------------------
TimeService::TimeService() {
    m_last = 0;
}
// ----------------------------------------------------------------------------
TimeService::~TimeService() {
}
// ----------------------------------------------------------------------------
bool TimeService::post_timer(TimerHandler<int>* handler, Timestamp tick, const int message) {
    return m_timequeue.post(handler, tick, message);
}
// ----------------------------------------------------------------------------
void TimeService::remove_timer(TimerHandler<int>* handler) {
    m_timequeue.remove(handler);
}
// ----------------------------------------------------------------------------
bool TimeService::timer() {
    bool handled = false;
    TimerHandler<int>* handler;
    int msg;
    Timestamp current_time = now();
    Timestamp dt = current_time - m_last;
    m_last = current_time;
    while (m_timequeue.get(&handler, dt, &msg)) {
        dt = 0;
        handled |= handler->cb_timer(current_time, msg);
    } return handled;
}
// ----------------------------------------------------------------------------
Timestamp TimeService::now() const {
    return (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();
}

// ----------------------------------------------------------------------------
