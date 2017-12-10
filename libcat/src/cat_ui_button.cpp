#include "cat_ui_button.h"
#include "cat_gfx_renderer.h"

using namespace cat;

// ----------------------------------------------------------------------------
Button::Button(KernelApi* kernel, const Rect2i& rect, unsigned int id) : Label(kernel, rect, id) {
    m_texrefs.resize(3);
    m_textstyle.appearance = TextStyle::Appearance::Bold | TextStyle::Appearance::Shadow;
    m_textstyle.gravity = TextStyle::Gravity::CenterHorizontal | TextStyle::Gravity::CenterVertical;
    m_textstyle.fontsize = 10;
    m_textstyle.color = 0xffffffff;
    m_pressed = false;
    m_checkable = false;
    m_checked = false;
}
// ----------------------------------------------------------------------------
Button::~Button() {
}
// ----------------------------------------------------------------------------
void Button::set_checked(bool b) {
    m_checked = b;
    ev_check.call(this, m_checked);
}
// ----------------------------------------------------------------------------
bool Button::cb_touch(const TouchEvent& ev, bool handled) {
    switch (ev.type) {
    case TouchEvent::EventType::TouchDown:
        if (handled || ev.pointer_id!=0) return false;
        if (!m_absrect.contain(ev.x, ev.y)) return false;
        m_checked = !m_checked;
        m_pressed = true;
        ev_check.call(this, m_checked);
        return true;
    case TouchEvent::EventType::TouchUp:
        if (ev.pointer_id!=0) return false;
        if (!m_pressed) return false;
        m_pressed = false;
        perform_click();
        return true;
    default:
        return false;
    }
}
// ----------------------------------------------------------------------------
void Button::cb_render(Renderer* r, Timestamp now) {
    int tex = TexNormal;
    if (m_pressed) tex = TexPressed;
    else if (m_checkable && m_checked!=0) tex = TexChecked;
    r->draw2d.fill(m_absrect, apply_opacity(m_bgcolor), m_texrefs[tex], now);
    r->draw2d.drawtext(m_absrect, m_text, m_textstyle);
}
// ----------------------------------------------------------------------------
