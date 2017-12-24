#ifndef __CAT_TIME_QUEUE_H__
#define __CAT_TIME_QUEUE_H__

#include <list>
#include <mutex>
#include "cat_time_type.h"

namespace cat {
// -----------------------------------------------------------
//! Delta queue implementation
template <class HANDLE, typename T>
class TimerQueue {
public:
    //! Post a timer to queue
    //! \param tick How long for time timer, in milliseconds
    //! \param handle handle of timer
    //! \param message user-defined message to pass to handler
    //! \return true if success, false if failed and no side-effect
    bool post(Timestamp tick, const HANDLE& handle, const T& message);
    // Get a due timer
    //! \param tick How long passed last call to get(), in milliseconds
    //! \param handle handle of timer
    //! \param message Receive the user-defined message of due timer
    //! \return true if there is timer due, false if no due timer
    bool get (Timestamp tick, HANDLE* handle, T* message);
    //! Remove all timer assocated with the handler
    //! Any timer not fired yet will be discarded
    //! \param handle handle of timer
    void remove(const HANDLE& handle);
    //! Remove all timer assocated with the handler with lambda condition
    //! Any timer not fired yet will be discarded
    //! \param comparator a function return true if the timer should be removed
    void remove(std::function<bool(const HANDLE& handle, const T& data)> comparator);

private:
    struct NODE {
        Timestamp tick;
        HANDLE    handle;
        T         message;
    };
    std::mutex      m_mutex;
    std::list<NODE> m_queue;
    unsigned long   m_tick;
};
// -----------------------------------------------------------
template <class HANDLE, typename T>
bool TimerQueue<HANDLE,T>::post(Timestamp tick, const HANDLE& handle, const T& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.size() == 0) {
        m_tick = 0;
    } else {
        for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
            if (tick < it->tick) {
                it->tick -= tick;
                m_queue.insert(it, NODE{ tick, handle, message });
                return true;
            } else {
                tick -= it->tick;
            }
        }
    }
    m_queue.push_back(NODE{ tick, handle, message });
    return true;
}
// -----------------------------------------------------------
template <class HANDLE, typename T>
bool TimerQueue<HANDLE,T>::get(Timestamp tick, HANDLE* handle, T* msg) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto node = m_queue.begin();
	if (node == m_queue.end()) return false;
    m_tick += tick;
    if (m_tick >= node->tick) {
        m_tick -= node->tick;
        *handle = node->handle;
        *msg = node->message;
        m_queue.erase(node);
        return true;
    } return false;
}
// -----------------------------------------------------------
template <class HANDLE, typename T>
void TimerQueue<HANDLE,T>::remove(const HANDLE& handle) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_queue.begin(); it != m_queue.end();) {
        if (it->handle == handle) {
            it = m_queue.erase(it);
        } else {
            ++it;
        }
    }
}
// -----------------------------------------------------------
template <class HANDLE, typename T>
void TimerQueue<HANDLE,T>::remove(std::function<bool(const HANDLE& handle, const T& data)> comparator) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_queue.begin(); it != m_queue.end();) {
        if (comparator(it->handle, it->message)) {
            it = m_queue.erase(it);
        } else {
            ++it;
        }
    }
}
// -----------------------------------------------------------
} // namespace cat

#endif // __CAT_TIME_QUEUE_H__
