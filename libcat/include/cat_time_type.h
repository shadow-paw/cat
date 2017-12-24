#ifndef __CAT_TIME_TYPE_H__
#define __CAT_TIME_TYPE_H__

#include <functional>

namespace cat {
// ----------------------------------------------------------------------------
typedef unsigned long Timestamp;
typedef std::function<void(Timestamp)> TimerFunction;
// -----------------------------------------------------------
template <typename T>
class TimerDelegate {
public:
    virtual bool cb_timer(Timestamp now, T msg) = 0;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_TIME_TYPE_H__
