#include "osal_ui_slider.h"
#include "osal_gfx_renderer.h"

using namespace osal;

// ----------------------------------------------------------------------------
Slider::Slider(KernelApi* kernel, const Rect2i& rect, unsigned int id) : Widget(kernel, rect, id) {
    m_texrefs.resize(2);
    m_orentation = Orentation::Horizontal;
    m_min = m_max = m_pos = 0;
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
    if (m_pos < min) {
        m_pos = min;
        ev_slide.call(this, m_pos);
        update_thumbrc();
    }
}
// ----------------------------------------------------------------------------
void Slider::set_max(int max) {
    if (m_max == max) return;
    m_max = max;
    if (m_min > max) m_min = max;
    if (m_pos > max) {
        m_pos = max;
        update_thumbrc();
        ev_slide.call(this, m_pos);
    }
}
// ----------------------------------------------------------------------------
void Slider::set_range(int min, int max) {
    if (max < min) max = min;
    if (min==m_min && max==m_max) return;
    m_min = min;
    m_max = max;
    if (m_pos < m_min) {
        m_pos = m_min;
        update_thumbrc();
        ev_slide.call(this, m_pos);
    } else if (m_pos > m_max) {
        m_pos = m_max;
        update_thumbrc();
        ev_slide.call(this, m_pos);
    }
}
// ----------------------------------------------------------------------------
void Slider::set_pos(int pos) {
    if (m_pos == pos) return;
    m_pos = pos;
    if (m_pos < m_min) {
        m_pos = m_min;
    } else if (m_pos > m_max) {
        m_pos = m_max;
    }
    update_thumbrc();
    ev_slide.call(this, m_pos);
}
// ----------------------------------------------------------------------------
int Slider::choose_pos(int x, int y) const {
    return 0;
}
// ----------------------------------------------------------------------------
void Slider::cb_resize() {
    update_thumbrc();
}
// ----------------------------------------------------------------------------
bool Slider::cb_touch(const TouchEvent& ev, bool handled) {
    switch (ev.type) {
    case TouchEvent::EventType::TouchDown:
        if (handled || ev.pointer_id != 0) break;
        if (!m_absrect.contain(ev.x, ev.y)) break;
        if (m_thumbrc.contain(ev.x, ev.y)) {
            m_dragx = ev.x;
            m_dragy = ev.y;
            m_dragging = true;
        } return true;
    case TouchEvent::EventType::TouchUp: {
        if (ev.pointer_id!=0) break;
        if (!m_dragging) break;
        m_dragging = false;
        if (m_min == m_max) break;
        update_thumbrc();
        return true;
    }
    case TouchEvent::EventType::TouchMove: {
        if (!m_dragging) break;
        if (m_min == m_max) break;
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
            int pos = offset != 0 ? (m_max - m_min+1) * offset / width + m_min : m_min;
            if (pos < m_min) pos = m_min;
            else if (pos > m_max) pos = m_max;
            if (m_pos!=pos) {
                m_pos = pos;
                ev_slide.call(this, m_pos);
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
            int pos = offset != 0 ? (m_max - m_min+1) * offset / height + m_min : m_min;
            if (pos < m_min) pos = m_min;
            else if (pos > m_max) pos = m_max;
            if (m_pos != pos) {
                m_pos = pos;
                ev_slide.call(this, m_pos);
            }
        }
        m_dragx = ev.x;
        m_dragy = ev.y;
        return true;
    }
    case TouchEvent::EventType::Scroll:
        if (!m_absrect.contain(ev.x, ev.y)) break;
        if (ev.scroll > 0) {
            if (m_pos <= m_min) break;
            m_pos --;
            update_thumbrc();
            ev_slide.call(this, m_pos);
            return true;
        } else if (ev.scroll < 0) {
            if (m_pos >= m_max) break;
            m_pos++;
            update_thumbrc();
            ev_slide.call(this, m_pos);
            return true;
        } break;
    } return false;
}
// ----------------------------------------------------------------------------
void Slider::update_thumbrc() {
    if (m_orentation == Orentation::Horizontal) {
        m_thumbrc.size.height = m_absrect.size.height;
        m_thumbrc.size.width = m_thumbrc.size.height;
        m_thumbrc.origin.x = m_absrect.origin.x;
        m_thumbrc.origin.y = m_absrect.origin.y;
        if (m_max != m_min) m_thumbrc.origin.x += m_pos * (m_absrect.size.width - m_thumbrc.size.width) / (m_max - m_min);
    } else {
        m_thumbrc.size.width = m_absrect.size.width;
        m_thumbrc.size.height = m_thumbrc.size.width;
        m_thumbrc.origin.x = m_absrect.origin.x;
        m_thumbrc.origin.y = m_absrect.origin.y;
        if (m_max != m_min) m_thumbrc.origin.y += m_pos * (m_absrect.size.height - m_thumbrc.size.height) / (m_max - m_min);
    }
}
// ----------------------------------------------------------------------------
void Slider::cb_render(Renderer* r, Timestamp now) {
    r->draw2d.fill(m_absrect, 0xffffffff, m_texrefs[TexBackground], now);
    r->draw2d.fill(m_thumbrc, 0xffffffff, m_texrefs[TexThumb], now);
}
// ----------------------------------------------------------------------------
