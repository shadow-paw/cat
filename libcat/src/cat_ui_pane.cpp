#include "cat_ui_pane.h"
#include "cat_gfx_renderer.h"
#include "cat_ui_service.h"

using namespace cat;

// ----------------------------------------------------------------------------
Pane::Pane(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Widget(kernel_api, rect, id) {
    m_texrefs.resize(1);
    m_draggable = false;
    m_dragging = false;
    m_bounded = false;
    m_clipping = true;
}
// ----------------------------------------------------------------------------
Pane::~Pane() {
}
// ----------------------------------------------------------------------------
bool Pane::cb_touch(const TouchEvent& ev, bool handled) {
    switch (ev.type) {
    case TouchEvent::EventType::TouchDown:
        if (handled || ev.pointer_id != 0 || !m_draggable) return false;
        if (!m_absrect.contain(ev.x, ev.y)) return false;
        m_dragging = true;
        m_dragx = ev.x;
        m_dragy = ev.y;
        return true;
    case TouchEvent::EventType::TouchUp:
        if (ev.pointer_id != 0) return false;
        if (!m_dragging) return false;
        m_dragging = false;
        return true;
    case TouchEvent::EventType::TouchMove: {
        if (!m_dragging) return false;
        m_rect.origin.x += ev.x - m_dragx;
        m_rect.origin.y += ev.y - m_dragy;
        m_dragx = ev.x;
        m_dragy = ev.y;
        if (m_bounded) {
            if (m_rect.origin.x < 0) m_rect.origin.x = 0;
            if (m_rect.origin.y < 0) m_rect.origin.y = 0;
            if (m_parent) {
                auto parent_size = m_parent->get_size();
                if (m_rect.origin.x > parent_size.width - m_rect.size.width) m_rect.origin.x = parent_size.width - m_rect.size.width;
                if (m_rect.origin.y > parent_size.height - m_rect.size.height) m_rect.origin.y = parent_size.height - m_rect.size.height;
            }
        }
        update_absrect();
        return true;
    }
    default:
        return false;
    }
}
// ----------------------------------------------------------------------------
bool Pane::cb_timer(Timestamp now, int code) {
    return true;
}
// ----------------------------------------------------------------------------
void Pane::cb_render(Renderer* r, Timestamp now) {
    r->draw2d.fill(m_absrect, apply_opacity(m_bgcolor), m_texrefs[TexBackground], now);
}
// ----------------------------------------------------------------------------
