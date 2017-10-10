#ifndef __OSAL_TIME_SERVICE_H__
#define __OSAL_TIME_SERVICE_H__

#include "osal_time_queue.h"

namespace osal { namespace time {
// -----------------------------------------------------------
class TimeService {
public:
    TimeService();
    ~TimeService();
    
    Timestamp now() const;
    
    bool post(TimerHandler<int>* handler, Timestamp, const int message);
    void remove(TimerHandler<int>* handler);
    bool timer();
private:
    time::TimerQueue<int> m_timequeue;
    Timestamp m_last;
};
// -----------------------------------------------------------
}} // namespace osal::time

#endif // __OSAL_TIME_SERVICE_H__
