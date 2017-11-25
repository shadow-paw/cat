#ifndef __CAT_DATA_UNIQUEID_H__
#define __CAT_DATA_UNIQUEID_H__

#include <deque>
#include <mutex>

namespace cat {
// ----------------------------------------------------------------------------
template <typename...> class UniqueId;
//! Unique ID Generator without timeout
template <typename T>
class UniqueId<T> {
public:
    //! Initialize the generator
    //! \param lo Lower limit of an id, inclusive
    //! \param hi Upper limit of an id, inclusive
    //! \param invalid id denote an invalid value
    //! \return true if success, false if failed and no side-effect
    bool init(const T& lo, const T& hi, const T& invalid);
    //! Reset the generator to initialized state
    void reset();
    //! Fetch a new id
    //! \param now Current time
    //! \return id
    T fetch();
    //! Release an id, the id may be reused after a certain timeout
    //! \param now Current time
    //! \param id id
    //! \return true if success, false if failed and no side-effect
    bool release(const T& id);
private:
    std::mutex    m_mutex;
    std::deque<T> m_list;
    T             m_limit_lo, m_limit_hi, m_seed, m_invalid;
};
// ----------------------------------------------------------------------------
//! Unique ID Generator with timeout
template <typename T, typename TIMESTAMP>
class UniqueId<T,TIMESTAMP> {
public:
    //! Initialize the generator
    //! \param lo Lower limit of an id, inclusive
    //! \param hi Upper limit of an id, inclusive
    //! \param invalid id denote an invalid value
    //! \param timeout Timeout before an id is reused
    //! \return true if success, false if failed and no side-effect
    bool init(const T& lo, const T& hi, const T& invalid, TIMESTAMP timeout);
    //! Reset the generator to initialized state
    void reset();
    //! Fetch a new id
    //! \param now Current time
    //! \return id
    T fetch(TIMESTAMP now);
    //! Release an id, the id may be reused after a certain timeout
    //! \param id id
    //! \param now Current time
    //! \return true if success, false if failed and no side-effect
    bool release(const T& id, TIMESTAMP now);
private:
    struct NODE {
        TIMESTAMP timestamp;
        T id;
    };
    std::mutex  m_mutex;
    std::deque<NODE> m_list;
    T                m_limit_lo, m_limit_hi, m_seed, m_invalid;
    TIMESTAMP        m_timeout;
};
// ----------------------------------------------------------------------------
// Unique ID Generator without timeout
// ----------------------------------------------------------------------------
template <typename T>
bool UniqueId<T>::init(const T& lo, const T& hi, const T& invalid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_limit_lo = lo;
    m_limit_hi = hi;
    m_seed = lo;
    m_invalid = invalid;
    return true;
}
// ----------------------------------------------------------------------------
template <typename T>
void UniqueId<T>::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_seed = m_limit_lo;
    m_list.clear();
}
// ----------------------------------------------------------------------------
template <typename T>
T UniqueId<T>::fetch() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_list.empty()) {
        auto id = m_list.front();
        m_list.pop_front();
        return id;
    }
    if (m_seed < m_limit_hi) {
        return m_seed++;
    } return m_invalid;
}
// ----------------------------------------------------------------------------
template <typename T>
bool UniqueId<T>::release(const T& id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_list.push_back(id);
    return true;
}
// ----------------------------------------------------------------------------
// Unique ID Generator with timeout
// ----------------------------------------------------------------------------
template <typename T, typename TIMESTAMP>
bool UniqueId<T, TIMESTAMP>::init(const T& lo, const T& hi, const T& invalid, TIMESTAMP timeout) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_timeout = timeout;
    m_limit_lo = lo;
    m_limit_hi = hi;
    m_seed = lo;
    m_invalid = invalid;
    return true;
}
// ----------------------------------------------------------------------------
template <typename T, typename TIMESTAMP>
void UniqueId<T, TIMESTAMP>::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_seed = m_limit_lo;
    m_list.clear();
}
// ----------------------------------------------------------------------------
template <typename T, typename TIMESTAMP>
T UniqueId<T, TIMESTAMP>::fetch(TIMESTAMP now) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_list.empty()) {
        NODE node = m_list.front();
        if (now - node.timestamp > m_timeout) {
            m_list.pop_front();
            return node.id;
        }
    }
    if (m_seed < m_limit_hi) {
        return m_seed++;
    } return m_invalid;
}
// ----------------------------------------------------------------------------
template <typename T, typename TIMESTAMP>
bool UniqueId<T, TIMESTAMP>::release(const T& id, TIMESTAMP now) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_list.push_back({ now, id });
    return true;
}
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_DATA_UNIQUEID_H__

