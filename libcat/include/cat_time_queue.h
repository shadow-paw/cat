#ifndef __CAT_TIME_QUEUE_H__
#define __CAT_TIME_QUEUE_H__

#include <list>
#include <mutex>
#include "cat_time_type.h"

namespace cat {
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
    void remove(std::function<bool(const TimerHandler<T>* handler, const T& data)> comparator);
private:
    struct NODE {
        Timestamp        tick;
        TimerHandler<T>* handler;
        T                message;
    };
    std::mutex      m_mutex;
    std::list<NODE> m_queue;
    unsigned long   m_tick;
};
// -----------------------------------------------------------
template <typename T>
bool TimerQueue<T>::post(TimerHandler<T>* handler, Timestamp tick, const T& message) {
    if (!handler) return false;
    std::lock_guard<std::mutex> lock(m_mutex);
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
    std::lock_guard<std::mutex> lock(m_mutex);
    auto node = m_queue.begin();
	if (node == m_queue.end()) return false;
    m_tick += tick;
    if (m_tick >= node->tick) {
        m_tick -= node->tick;
        *handler = node->handler;
        *msg = node->message;
        m_queue.erase(node);
        return true;
    } return false;
}
// -----------------------------------------------------------
template <typename T>
void TimerQueue<T>::remove(TimerHandler<T>* handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_queue.begin(); it != m_queue.end();) {
        if (it->handler == handler) {
            it = m_queue.erase(it);
        } else {
            ++it;
        }
    }
}
// -----------------------------------------------------------
template <typename T>
void TimerQueue<T>::remove(std::function<bool(const TimerHandler<T>* handler, const T& data)> comparator) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_queue.begin(); it != m_queue.end();) {
        if (comparator(it->handler, it->message)) {
            it = m_queue.erase(it);
        } else {
            ++it;
        }
    }
}
// -----------------------------------------------------------
} // namespace cat

#endif // __CAT_TIME_QUEUE_H__
