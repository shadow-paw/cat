#ifndef __CAT_UI_BUTTON_H__
#define __CAT_UI_BUTTON_H__

#include "cat_ui_label.h"
#include "cat_gfx_type.h"

namespace cat {
// ----------------------------------------------------------------------------
class Button : public Label {
public:
    static const int TexNormal = 0;
    static const int TexPressed = 1;
    static const int TexChecked = 2;

    // Event Handlers
    UIHandler<bool> ev_check;

    Button(KernelApi* kernel, const Rect2i& rect, unsigned int id = 0);
    virtual ~Button();

    void set_checkable(bool b) { m_checkable = b; }
    bool is_checkable() const { return m_checkable; }
    void set_checked(bool b);
    bool is_checked() const { return m_checked; }

protected:
    virtual bool cb_touch(const TouchEvent& ev, bool handled);
    virtual void cb_render(Renderer* r, Timestamp now);

protected:
    bool m_checkable;
    bool m_checked, m_pressed;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_BUTTON_H__
