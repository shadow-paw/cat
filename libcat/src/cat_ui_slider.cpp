#include "cat_ui_slider.h"
#include "cat_gfx_renderer.h"

using namespace cat;

// ----------------------------------------------------------------------------
Slider::Slider(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Widget(kernel_api, rect, id) {
    m_texrefs.resize(2);
    m_orentation = Orentation::Horizontal;
    m_min = m_max = m_value = 0;
    m_dragging = false;
    update_thumbrc();
}
// ----------------------------------------------------------------------------
Slider::~Slider() {
}
// ----------------------------------------------------------------------------
void Slider::set_orentation(Orentation o) {
    if (m_orentation == o) return;
    m_orentation = o;
}
// ----------------------------------------------------------------------------
void Slider::set_min(int min) {
    if (m_min == min) return;
    m_min = min;
    if (m_max < min) m_max = min;
    if (m_value < min) {
        m_value = min;
        ev_slide.call(this, m_value);
        update_thumbrc();
    }
}
// ----------------------------------------------------------------------------
void Slider::set_max(int max) {
    if (m_max == max) return;
    m_max = max;
    if (m_min > max) m_min = max;
    if (m_value > max) {
        m_value = max;
        update_thumbrc();
        ev_slide.call(this, m_value);
    }
}
// ----------------------------------------------------------------------------
void Slider::set_range(int min, int max) {
    if (max < min) max = min;
    if (min==m_min && max==m_max) return;
    m_min = min;
    m_max = max;
    if (m_value < m_min) {
        m_value = m_min;
        update_thumbrc();
        ev_slide.call(this, m_value);
    } else if (m_value > m_max) {
        m_value = m_max;
        update_thumbrc();
        ev_slide.call(this, m_value);
    }
}
// ----------------------------------------------------------------------------
void Slider::set_value(int value) {
    if (m_value == value) return;
    m_value = value;
    if (m_value < m_min) {
        m_value = m_min;
    } else if (m_value > m_max) {
        m_value = m_max;
    }
    update_thumbrc();
    ev_slide.call(this, m_value);
}
// ----------------------------------------------------------------------------
void Slider::cb_move() {
    update_thumbrc();
}
// ----------------------------------------------------------------------------
void Slider::cb_resize() {
    update_thumbrc();
}
// ----------------------------------------------------------------------------
bool Slider::cb_touch(const TouchEvent& ev, bool handled) {
    switch (ev.type) {
    case TouchEvent::EventType::TouchDown:
        if (handled || ev.pointer_id != 0) return false;
        if (!m_absrect.contain(ev.x, ev.y)) return false;
        if (m_thumbrc.contain(ev.x, ev.y)) {
            m_dragx = ev.x;
            m_dragy = ev.y;
            m_dragging = true;
        } return true;
    case TouchEvent::EventType::TouchUp: {
        if (ev.pointer_id!=0) return false;
        if (!m_dragging) return false;
        m_dragging = false;
        if (m_min != m_max) update_thumbrc();
        return true;
    }
    case TouchEvent::EventType::TouchMove: {
        if (!m_dragging) return false;
        if (m_min == m_max) return false;
        if (m_orentation == Orentation::Horizontal) {
            int dx = ev.x - m_dragx;
            m_thumbrc.origin.x += dx;
            if (m_thumbrc.origin.x < m_absrect.origin.x) {
                m_thumbrc.origin.x = m_absrect.origin.x;
            } else if (m_thumbrc.origin.x > m_absrect.origin.x + m_absrect.size.width - m_thumbrc.size.width) {
                m_thumbrc.origin.x = m_absrect.origin.x + m_absrect.size.width - m_thumbrc.size.width;
            }
            int width = m_absrect.size.width - m_thumbrc.size.width;
            int offset = m_thumbrc.origin.x - m_absrect.origin.x;
            int value = offset != 0 ? (m_max - m_min+1) * offset / width + m_min : m_min;
            if (value < m_min) value = m_min;
            else if (value > m_max) value = m_max;
            if (m_value!= value) {
                m_value = value;
                ev_slide.call(this, m_value);
            }
        } else {
            int dy = ev.y - m_dragy;
            m_thumbrc.origin.y += dy;
            if (m_thumbrc.origin.y < m_absrect.origin.y) {
                m_thumbrc.origin.y = m_absrect.origin.y;
            } else if (m_thumbrc.origin.y > m_absrect.origin.y + m_absrect.size.height - m_thumbrc.size.height) {
                m_thumbrc.origin.y = m_absrect.origin.y + m_absrect.size.height - m_thumbrc.size.height;
            }
            int height = m_absrect.size.height - m_thumbrc.size.height;
            int offset = m_thumbrc.origin.y - m_absrect.origin.y;
            int value = offset != 0 ? (m_max - m_min+1) * offset / height + m_min : m_min;
            if (value < m_min) value = m_min;
            else if (value > m_max) value = m_max;
            if (m_value != value) {
                m_value = value;
                ev_slide.call(this, m_value);
            }
        }
        m_dragx = ev.x;
        m_dragy = ev.y;
        return true;
    }
    case TouchEvent::EventType::Scroll:
        if (!m_absrect.contain(ev.x, ev.y)) return false;
        if (ev.scroll > 0) {
            if (m_value <= m_min) return false;
            m_value--;
            update_thumbrc();
            ev_slide.call(this, m_value);
            return true;
        } else if (ev.scroll < 0) {
            if (m_value >= m_max) return false;
            m_value++;
            update_thumbrc();
            ev_slide.call(this, m_value);
            return true;
        } return false;
    default:
        return false;
    }
}
// ----------------------------------------------------------------------------
void Slider::update_thumbrc() {
    if (m_orentation == Orentation::Horizontal) {
        m_thumbrc.size.height = m_absrect.size.height;
        m_thumbrc.size.width = m_thumbrc.size.height;
        m_thumbrc.origin.x = m_absrect.origin.x;
        m_thumbrc.origin.y = m_absrect.origin.y;
        if (m_max != m_min) m_thumbrc.origin.x += m_value * (m_absrect.size.width - m_thumbrc.size.width) / (m_max - m_min);
    } else {
        m_thumbrc.size.width = m_absrect.size.width;
        m_thumbrc.size.height = m_thumbrc.size.width;
        m_thumbrc.origin.x = m_absrect.origin.x;
        m_thumbrc.origin.y = m_absrect.origin.y;
        if (m_max != m_min) m_thumbrc.origin.y += m_value * (m_absrect.size.height - m_thumbrc.size.height) / (m_max - m_min);
    }
}
// ----------------------------------------------------------------------------
void Slider::cb_render(Renderer* r, Timestamp now) {
    r->draw2d.fill(m_absrect, apply_opacity(0xffffffff), m_texrefs[TexBackground], now);
    r->draw2d.fill(m_thumbrc, apply_opacity(0xffffffff), m_texrefs[TexThumb], now);
}
// ----------------------------------------------------------------------------
