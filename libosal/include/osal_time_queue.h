#ifndef __OSAL_TIME_QUEUE_H__
#define __OSAL_TIME_QUEUE_H__

#include <list>
#include "osal_time_type.h"

namespace osal {
// -----------------------------------------------------------
template <typename T>
class TimerHandler {
public:
    virtual bool cb_timer(Timestamp now, T msg) = 0;
};
// -----------------------------------------------------------
template <typename T>
class TimerQueue {
public:
    bool post(TimerHandler<T>* handler, Timestamp tick, const T& message);
    bool get (TimerHandler<T>** handler, Timestamp tick, T* message);
    void remove(TimerHandler<T>* handler);
private:
    struct NODE {
        Timestamp        tick;
        TimerHandler<T>* handler;
        T                message;
    };
    std::list<NODE> m_queue;
    unsigned long   m_tick;
};
// -----------------------------------------------------------
template <typename T>
bool TimerQueue<T>::post(TimerHandler<T>* handler, Timestamp tick, const T& message) {
    if (!handler) return false;
    if (m_queue.size() == 0) {
        m_tick = 0;
    } else {
        for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
            if (tick < it->tick) {
                it->tick -= tick;
                m_queue.insert(it, NODE{ tick, handler, message });
                return true;
            } else {
                tick -= it->tick;
            }
        }
    }
    m_queue.push_back(NODE{ tick, handler, message });
    return true;
}
// -----------------------------------------------------------
template <typename T>
bool TimerQueue<T>::get(TimerHandler<T>** handler, Timestamp tick, T* msg) {
	auto node = m_queue.begin();
	if (node == m_queue.end()) return false;
    m_tick += tick;
    if (m_tick >= node->tick) {
        m_tick -= node->tick;
        *handler = node->handler;
        *msg = std::move(node->message);
        m_queue.erase(node);
        return true;
    } return false;
}
// -----------------------------------------------------------
template <typename T>
void TimerQueue<T>::remove(TimerHandler<T>* handler) {
    for (auto it = m_queue.begin(); it != m_queue.end();) {
        if (it->handler == handler) {
            it = m_queue.erase(it);
        } else {
            ++it;
        }
    }
}
// -----------------------------------------------------------
} // namespace osal

#endif // __OSAL_TIME_QUEUE_H__
