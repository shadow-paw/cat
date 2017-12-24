#include "cat_time_service.h"
#include <chrono>

using namespace cat;

// ----------------------------------------------------------------------------
TimeService::TimeService() {
    m_last = now();
}
// ----------------------------------------------------------------------------
TimeService::~TimeService() {
}
// ----------------------------------------------------------------------------
void TimeService::pause() {
}
// ----------------------------------------------------------------------------
void TimeService::resume() {
    m_last = now();
}
// ----------------------------------------------------------------------------
Timestamp TimeService::now() {
    return (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
}
// ----------------------------------------------------------------------------
// TimerDelegate
// ----------------------------------------------------------------------------
bool TimeService::post_timer(TimerDelegate<int>* handler, int message, Timestamp tick) {
    if (!handler) return false;
    return m_delegates.post(tick, handler, message);
}
// ----------------------------------------------------------------------------
void TimeService::remove_timer(TimerDelegate<int>* handler) {
    if (!handler) return;
    m_delegates.remove(handler);
}
// ----------------------------------------------------------------------------
void TimeService::remove_timer(TimerDelegate<int>* handler, int message) {
    m_delegates.remove([handler,message](const TimerDelegate<int>* h, const int& m) -> bool {
        return handler==h && message==m;
    });
}
// ----------------------------------------------------------------------------
// lambda function
// ----------------------------------------------------------------------------
bool TimeService::post_timer(TimerFunction handler, Timestamp tick) {
    return m_functions.post(tick, handler, 0);
}
// ----------------------------------------------------------------------------
void TimeService::remove_timer(TimerFunction handler) {
    auto handler_p = handler.template target<TimerFunction>();
    m_functions.remove([handler_p](const TimerFunction& h, const int& m) -> bool {
        auto h_p = h.template target<TimerFunction>();
        return handler_p == h_p;
    });
}
// ----------------------------------------------------------------------------
bool TimeService::timer() {
    bool handled = false;
    TimerDelegate<int>* dg;
    TimerFunction func;
    int msg;
    Timestamp current_time = now();
    Timestamp dt = current_time - m_last;
    while (m_delegates.get(dt, &dg, &msg)) {
        dt = 0;
        handled |= dg->cb_timer(current_time, msg);
    }
    dt = current_time - m_last;
    while (m_functions.get(dt, &func, &msg)) {
        dt = 0;
        func(current_time);
        handled = true;
    }
    m_last = current_time;
    return handled;
}
// ----------------------------------------------------------------------------
