#ifndef __CAT_DATA_OBSERVABLE_H__
#define __CAT_DATA_OBSERVABLE_H__

#include <functional>
#include <list>

namespace cat {
// ----------------------------------------------------------------------------
template <class T>
using Observer = std::function<void(const T& data)>;
// ----------------------------------------------------------------------------
template <class T> class Observable;
template <class T>
class Cancellable {
public:
    Cancellable(Observable<T>* observable, Observer<T> observer) {
        m_observable = observable;
        m_observer = observer;
    }
    void cancel() {
        if (m_observable) {
            m_observable->unsubscribe(m_observer);
            m_observable = nullptr;
        } m_observer = nullptr;
    }
private:
    Observable<T>* m_observable;
    Observer<T> m_observer;
};
// ----------------------------------------------------------------------------
template <class T>
class Observable {
public:
    Cancellable<T> subscribe(Observer<T> observer);
    void unsubscribe(Observer<T> observer);
    void notify();
    T& data() { return m_data; }

private:
    T m_data;
    std::list<Observer<T>> m_observers;
};
// ----------------------------------------------------------------------------
template <class T>
Cancellable<T> Observable<T>::subscribe(Observer<T> observer) {
    m_observers.push_back(observer);
    return Cancellable<T>(this, observer);
}
// ----------------------------------------------------------------------------
template <class T>
void Observable<T>::unsubscribe(Observer<T> observer) {
    for (auto it = m_observers.begin(); it != m_observers.end();) {
        if (*(size_t*)&(*it) == *(size_t*)&observer) {  // erase if underlying function addresses are equal
            it = m_observers.erase(it);
        } else {
            ++ it;
        }
    }
}
// ----------------------------------------------------------------------------
template <class T>
void Observable<T>::notify() {
    for (auto it = m_observers.begin(); it != m_observers.end(); ++it) {
        (*it)(m_data);
    }
}
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_DATA_OBSERVABLE_H__
