#ifndef __CAT_UI_HANDLER_H__
#define __CAT_UI_HANDLER_H__

#include <functional>
#include <vector>

namespace cat {
class Widget;
// ----------------------------------------------------------------------------
template <class... ARG>
class UIHandler {
public:
    typedef std::function<void(Widget*, const ARG&...)> HANDLER;

    void operator = (const HANDLER& handler) {
        m_handler = handler;
    }
    bool call(Widget* widget, const ARG&... args) {
        if (m_handler) {
            m_handler(widget, args...);
            return true;
        } return false;
    }
private:
    HANDLER m_handler;
};
// ----------------------------------------------------------------------------
template <class... ARG>
class UIHandlers {
public:
    typedef std::function<void(Widget*, const ARG&...)> HANDLER;

    void operator += (const HANDLER& handler) {
        m_handlers.push_back({handler, true});
    }
    void operator -= (const HANDLER& handler) {
        auto handler_p = handler.template target<HANDLER>();
        for (auto& node: m_handlers) {
            if (handler_p == node.handler.template target<HANDLER>()) {
                node.active = false;
            }
        }
    }
    bool call(Widget* widget, const ARG&... args) {
        bool handled = false;
        for (auto it=m_handlers.begin(); it!=m_handlers.end();) {
            if (it->active) {
                handled = true;
                it->handler(widget, args...);
                ++it;
            } else {
                it = m_handlers.erase(it);
            }
        } return handled;
    }
private:
    struct NODE {
        HANDLER handler;
        bool active;
    };
    std::vector<NODE> m_handlers;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_HANDLER_H__
