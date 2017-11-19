#ifndef __CAT_DATA_OBSERVABLE_H__
#define __CAT_DATA_OBSERVABLE_H__

#include <functional>
#include <list>
#include "cat_data_uniqueid.h"

namespace cat {
// ----------------------------------------------------------------------------
template <class T>
using Observer = std::function<void(const T& data)>;
// ----------------------------------------------------------------------------
template <class T> class Observable;
template <class T>
class ObservableCanceller {
public:
    ObservableCanceller(Observable<T>* observable, int subscribe_id) {
        m_observable = observable;
        m_subscribe_id = subscribe_id;
    }
    void cancel() {
        if (m_observable) {
            m_observable->unsubscribe(m_subscribe_id);
            m_observable = nullptr;
        } m_subscribe_id = 0;
    }
private:
    Observable<T>* m_observable;
    int            m_subscribe_id;
};
// ----------------------------------------------------------------------------
template <class T>
class Observable {
public:
    Observable();

          T& data()       { return m_data; }
    const T& data() const { return m_data; }

    ObservableCanceller<T> subscribe(Observer<T> observer);
    void unsubscribe(int subscribe_id);
    void notify();

    template <class MAP>
    ObservableCanceller<T> distinct(std::function<MAP(const T& data)> mapper, Observer<MAP> observer);
private:
    struct SUBSCRIPTION {
        int id;
        Observer<T> observer;
        std::function<void()> cb_cancel;
    };
    T m_data;
    std::list<SUBSCRIPTION> m_subs;
    UniqueId<int> m_ids;
private:
    ObservableCanceller<T> subscribe(Observer<T> observer, std::function<void()> cb_cancel);
};
// ----------------------------------------------------------------------------
template <class T>
Observable<T>::Observable() {
    m_ids.init(0, 1, 0x7fffffff);
}
// ----------------------------------------------------------------------------
template <class T>
ObservableCanceller<T> Observable<T>::subscribe(Observer<T> observer) {
    return subscribe(observer, nullptr);
}
// ----------------------------------------------------------------------------
template <class T>
ObservableCanceller<T> Observable<T>::subscribe(Observer<T> observer, std::function<void()> cb_cancel) {
    auto id = m_ids.fetch(0);
    SUBSCRIPTION sub;
    sub.id = id;
    sub.observer = observer;
    sub.cb_cancel = cb_cancel;
    m_subs.push_back(sub);
    return ObservableCanceller<T>(this, id);
}
// ----------------------------------------------------------------------------
template <class T>
void Observable<T>::unsubscribe(int subscribe_id) {
    for (auto it = m_subs.begin(); it != m_subs.end();) {
        if (it->id == subscribe_id) {
            if (it->cb_cancel) it->cb_cancel();
            it = m_subs.erase(it);
            m_ids.release(0, subscribe_id);
        } else {
            ++it;
        }
    }
}
// ----------------------------------------------------------------------------
template <class T>
void Observable<T>::notify() {
    for (auto it = m_subs.begin(); it != m_subs.end(); ++it) {
        it->observer(m_data);
    }
}
// ----------------------------------------------------------------------------
template <class T>
template <class MAP>
ObservableCanceller<T> Observable<T>::distinct(std::function<MAP(const T& data)> mapper, Observer<MAP> observer) {
    MAP* mapped = new MAP();
    return subscribe(
        [mapped, mapper, observer](const T& data) -> void {
            MAP new_mapped = mapper(data);
            if (*mapped != new_mapped) {
                *mapped = new_mapped;
                observer(*mapped);
            }
        },
        [mapped]() -> void {
            delete mapped;
        }
    );
}
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_DATA_OBSERVABLE_H__
