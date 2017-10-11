#ifndef __OSAL_UI_HANDLER_H__
#define __OSAL_UI_HANDLER_H__

#include <functional>
#include <list>

namespace osal {
class Widget;
// ----------------------------------------------------------------------------
template <class... ARG>
class EventHandlers {
public:
    typedef std::function<bool(Widget*, const ARG&...)> HANDLER;

    void operator += (HANDLER handler) {
        m_handlers.push_back(handler);
    }
    void operator -= (HANDLER handler) {
        m_handlers.remove(handler);
    }
    bool call(Widget* widget, const ARG&... args) {
        bool handled = false;
        for (auto& hander : m_handlers) {
            handled |= hander(widget, args...);
        } return handled;
    }
private:
    std::list<HANDLER> m_handlers;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_UI_HANDLER_H__
