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
    typedef std::function<bool(Widget*, const ARG&...)> HANDLER;

    UIHandler() {
        m_handler = nullptr;
    }
    void operator = (const HANDLER& handler) {
        m_handler = handler;
    }
    bool call(Widget* widget, const ARG&... args) {
        return m_handler && m_handler(widget, args...);
    }
private:
    HANDLER m_handler;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_HANDLER_H__
