#ifndef __OSAL_UI_BUTTON_H__
#define __OSAL_UI_BUTTON_H__

#include "osal_ui_label.h"
#include "osal_gfx_type.h"

namespace osal { namespace ui {
// ----------------------------------------------------------------------------
class Button : public Label {
public:
    static const int TexNormal = 0;
    static const int TexPressed = 1;
    static const int TexChecked = 2;

    // Event Handlers
    EventHandlers<bool> ev_check;

    Button(osal::KernelApi* kernel, const osal::gfx::Rect2i& rect, unsigned int id = 0);
    virtual ~Button();

    void set_checkable(bool b) { m_checkable = b; }
    bool is_checkable() const { return m_checkable; }
    void set_checked(bool b);
    bool is_checked() const { return m_checked; }

protected:
    virtual bool cb_touch(const TouchEvent& ev, bool handled);
    virtual void cb_render(osal::gfx::Renderer* r, osal::time::Timestamp now);

protected:
    bool m_checkable;
    bool m_checked, m_pressed;
};
// ----------------------------------------------------------------------------
}} // namespace osal::ui

#endif // __OSAL_UI_BUTTON_H__
