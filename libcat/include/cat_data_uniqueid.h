#ifndef __CAT_DATA_UNIQUEID_H__
#define __CAT_DATA_UNIQUEID_H__

#include <deque>
#include <mutex>
#include "cat_time_type.h"

namespace cat {
// ----------------------------------------------------------------------------
template <typename T>
class UniqueId {
public:
    UniqueId();
    bool init(Timestamp timeout, const T& lo, const T& hi);
    void reset();
    T    fetch(Timestamp now);
    bool release(Timestamp now, const T& id);
private:
    struct NODE {
        Timestamp timestamp;
        T id;
    };
    std::deque<NODE> m_list;
    Timestamp        m_timeout;
    T                m_limit_lo, m_limit_hi, m_seed;
};
// ----------------------------------------------------------------------------
template <typename T>
class UniqueId_r {
public:
    bool init(Timestamp timeout, const T& lo, const T& hi) {
        return m_uniqueid.init(timeout, lo, hi);
    }
    void reset() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_uniqueid.reset();
    }
    T fetch(Timestamp now) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_uniqueid.fetch(now);
    }
    bool release(Timestamp now, const T& id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_uniqueid.release(now, id);
    }
private:
    std::mutex  m_mutex;
    UniqueId<T> m_uniqueid;
};
// ----------------------------------------------------------------------------
template <typename T>
UniqueId<T>::UniqueId() : m_limit_lo(), m_limit_hi(), m_seed() {
    m_timeout = 0;
}
// ----------------------------------------------------------------------------
template <typename T>
bool UniqueId<T>::init(Timestamp timeout, const T& lo, const T& hi) {
    m_timeout = timeout;
    m_limit_lo = lo;
    m_limit_hi = hi;
    m_seed = lo;
    return true;
}
// ----------------------------------------------------------------------------
template <typename T>
void UniqueId<T>::reset() {
    m_seed = m_limit_lo;
    m_list.clear();
}
// ----------------------------------------------------------------------------
template <typename T>
T UniqueId<T>::fetch(Timestamp now) {
    if (!m_list.empty()) {
        NODE node = m_list.front();
        if (now - node.timestamp > m_timeout) {
            m_list.pop_front();
            return node.id;
        }
    }
    if (m_seed < m_limit_hi) {
        return m_seed++;
    } return T();
}
// ----------------------------------------------------------------------------
template <typename T>
bool UniqueId<T>::release(Timestamp now, const T& id) {
    m_list.push_back({ now, id });
    return true;
}
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_DATA_UNIQUEID_H__

