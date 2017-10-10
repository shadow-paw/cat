#ifndef __OSAL_TIME_SERVICE_H__
#define __OSAL_TIME_SERVICE_H__

#include "osal_time_queue.h"

// Forward declaration, sadly c++ does not allowg short class gfx::Renderer;
// ----------------------------------------------------------------------------
namespace osal {
    class Kernel;
}

namespace osal { namespace time {
// ----------------------------------------------------------------------------
class TimeService {
friend class osal::Kernel;
public:
    TimeService();
    ~TimeService();
    
    Timestamp now() const;
    
    bool post_timer(TimerHandler<int>* handler, Timestamp, const int message);
    void remove_timer(TimerHandler<int>* handler);

private:
    // called from kernel
    bool timer();

private:
    time::TimerQueue<int> m_timequeue;
    Timestamp m_last;
};
// ----------------------------------------------------------------------------
}} // namespace osal::time

#endif // __OSAL_TIME_SERVICE_H__
