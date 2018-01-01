#ifndef __CAT_DATA_EVENTHANDLER_H__
#define __CAT_DATA_EVENTHANDLER_H__

#include <functional>
#include <vector>

namespace cat {
class Widget;
// ----------------------------------------------------------------------------
template <class OBJ, class... ARG>
class EventHandler {
public:
    typedef std::function<void(OBJ*, const ARG&...)> HANDLER;

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
    bool call(OBJ* obj, const ARG&... args) {
        bool handled = false;
        for (auto it=m_handlers.begin(); it!=m_handlers.end();) {
            if (it->active) {
                handled = true;
                it->handler(obj, args...);
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

#endif // __CAT_DATA_EVENTHANDLER_H__
