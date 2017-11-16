#ifndef __CAT_TIME_SERVICE_H__
#define __CAT_TIME_SERVICE_H__

#include "cat_time_queue.h"

namespace cat {
class Kernel;
// ----------------------------------------------------------------------------
class TimeService {
friend class cat::Kernel;
public:
    TimeService();
    ~TimeService();
    
    static Timestamp now();

    bool post_timer(TimerHandler<int>* handler, int message, Timestamp tick);
    void remove_timer(TimerHandler<int>* handler);
    void remove_timer(TimerHandler<int>* handler, int message);

private:
    // called from kernel
    bool timer();
    void pause();
    void resume();

private:
    TimerQueue<int> m_timequeue;
    Timestamp m_last, m_tick;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_TIME_SERVICE_H__
