#include "cat_ui_widget.h"
#include "cat_ui_service.h"
#include "cat_time_service.h"
#include "cat_gfx_renderer.h"
#include "cat_gfx_resmgr.h"
#include "cat_gfx_draw2d.h"
#include "cat_util_log.h"

using namespace cat;

// ----------------------------------------------------------------------------
Widget::Widget(KernelApi* kernel, const Rect2i& rect, unsigned int id) : animators(this) {
    m_kernel = kernel;
    m_parent = nullptr;
    m_id = id;
    m_bgcolor = 0xffffffff;
    m_opacity = m_absopacity = 1.0f;
    m_rect = rect;
    m_absrect = m_rect;
    m_visible = true;
    m_enable = true;
    m_clipping = false;
}
// ----------------------------------------------------------------------------
Widget::~Widget() {
    remove_timer();
    remove_childs();
    for (auto& texref : m_texrefs) {
        if (texref.tex) m_kernel->res()->release_tex(texref.tex);
    }
    m_texrefs.clear();
}
// ----------------------------------------------------------------------------
bool Widget::attach(Widget* child) {
    if (child->m_parent) {
        Logger::e("osal", "UI.attach - widget already has a parent!");
        return false;
    }
    child->m_parent = this;
    m_childs.push_back(child);
    update_absrect();
    // If visibility changed
    bool visible = child->is_visible();
    for (auto parent = this; parent; parent = parent->m_parent) {
        visible &= parent->is_visible();
    }
    if (visible != child->is_visible()) {
        child->notify_visible(visible);
    } return true;
}
// ----------------------------------------------------------------------------
void Widget::detach(Widget* child) {
    child->m_parent = this;
    for (auto it = m_childs.begin(); it != m_childs.end();) {
        if (*it == child) {
            it = m_childs.erase(it);
        } else {
            ++ it;
        }
    }
}
// ----------------------------------------------------------------------------
void Widget::remove_childs() {
    for (auto& child : m_childs) {
        delete child;
    } m_childs.clear();
}
// ----------------------------------------------------------------------------
Widget* Widget::child_at(unsigned int index) const {
    return index < m_childs.size() ? m_childs[index] : nullptr;
}
// ----------------------------------------------------------------------------
Widget* Widget::child_withid(unsigned int id) const {
    for (auto& child : m_childs) {
        if (child->m_id == id) return child;
    } return nullptr;
}
// ----------------------------------------------------------------------------
void Widget::update_absrect() {
    m_absrect = m_rect;
    if (m_parent) {
        m_absrect.origin.x += m_parent->m_absrect.origin.x;
        m_absrect.origin.y += m_parent->m_absrect.origin.y;
    }
    for (auto& child : m_childs) {
        child->update_absrect();
    }
    cb_resize();
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::set_pos(int x, int y) {
    m_rect.origin.x = x;
    m_rect.origin.y = y;
    update_absrect();
    cb_resize();
}
// ----------------------------------------------------------------------------
void Widget::set_pos(const Point2i& pos) {
    m_rect.origin = pos;
    update_absrect();
    cb_resize();
}
// ----------------------------------------------------------------------------
void Widget::set_size(int width, int height) {
    m_rect.size.width = width;
    m_rect.size.height = height;
    m_absrect.size.width = width;
    m_absrect.size.height = height;
    cb_resize();
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::set_size(const Size2i& size) {
    m_rect.size = size;
    m_absrect.size = size;
    cb_resize();
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::set_visible(bool b) {
    if (m_visible == b) return;
    m_visible = b;
    notify_visible(b);
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::set_enable(bool b) {
    if (m_enable == b) return;
    m_enable = b;
    notify_enable(b);
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::set_bgcolor(uint32_t color) {
    if (m_bgcolor == color) return;
    m_bgcolor = color;
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::bring_tofront() {
    if (!m_parent) return;
    auto parent = m_parent;
    parent->detach(this);
    parent->m_childs.push_back(this);
    m_parent = parent;
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::set_texture(unsigned int index, const std::string& name, int u0, int v0, int u1, int v1, int border_u, int border_v) {
    return set_texture(index, name.c_str(), u0, v0, u1, v1, border_u, border_v);
}
// ----------------------------------------------------------------------------
void Widget::set_texture(unsigned int index, const char* name, int u0, int v0, int u1, int v1, int border_u, int border_v) {
    if (index >= m_texrefs.size()) return;
    const Texture* oldtex = m_texrefs[index].tex;
    m_texrefs[index].tex = name ? m_kernel->res()->retain_tex(name) : nullptr;
    m_texrefs[index].u1 = u0;
    m_texrefs[index].v1 = v0;
    m_texrefs[index].u2 = u1;
    m_texrefs[index].v2 = v1;
    m_texrefs[index].border_u = border_u;
    m_texrefs[index].border_v = border_v;
    if (oldtex) m_kernel->res()->release_tex(oldtex);
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::notify_uiscaled() {
    for (auto& child : m_childs) {
        child->notify_uiscaled();
    } cb_uiscale();
}
// ----------------------------------------------------------------------------
void Widget::notify_visible(bool b) {
    for (auto& child : m_childs) {
        child->notify_visible(b & child->is_visible());
    } cb_visible(b);
}
// ----------------------------------------------------------------------------
void Widget::notify_enable(bool b) {
    for (auto& child: m_childs) {
        child->notify_enable(b & child->is_enabled());
    } cb_enable(b);
}
// ----------------------------------------------------------------------------
bool Widget::touch(const TouchEvent& ev, bool handled) {
    if (!m_visible || !m_enable) return false;
    for (auto it = m_childs.rbegin(); it != m_childs.rend(); ++it) {
        handled |= (*it)->touch(ev, handled);
    }
    handled |= cb_touch(ev, handled);
    return handled;
}
// ----------------------------------------------------------------------------
void Widget::render(Renderer* r, Timestamp now) {
    bool is_dirty = animators.translate.update(now);
    is_dirty |= animators.opacity.update(now);
    if (is_dirty) dirty();

    if (!m_visible) return;
    if (m_clipping) {
        r->draw2d.clipping_on(m_absrect);
        cb_render(r, now);
        r->draw2d.clipping_off();
    } else {
        cb_render(r, now);
    }
    for (auto& child: m_childs) {
        child->render(r, now);
    }
}
// ----------------------------------------------------------------------------
void Widget::post_timer(Timestamp delay, int code) {
    m_kernel->time()->post_timer(this, code, delay);
}
// ----------------------------------------------------------------------------
void Widget::remove_timer() {
    m_kernel->time()->remove_timer(this);
}
// ----------------------------------------------------------------------------
void Widget::capture(Texture& tex, const Rect2i& rect) {
    m_kernel->ui()->capture(tex, rect);
}
// ----------------------------------------------------------------------------
Draw2D* Widget::draw2d() {
    return &(m_kernel->renderer()->draw2d);
}
// ----------------------------------------------------------------------------
void Widget::dirty() {
    m_kernel->renderer()->dirty();
}
// ----------------------------------------------------------------------------
bool Widget::perform_click() {
    return ev_click.call(this);
}
// ----------------------------------------------------------------------------
void Widget::set_opacity(float opacity) {
    m_opacity = opacity;
    m_absopacity = m_opacity;
    for (auto parent = m_parent; parent; parent = parent->m_parent) {
        m_absopacity *= parent->get_opacity();
    }
    for (auto& child : m_childs) {
        child->update_absopacity(m_absopacity);
    }
    dirty();
}
// ----------------------------------------------------------------------------
void Widget::update_absopacity(float parent_absopacity) {
    m_absopacity = m_opacity * parent_absopacity;
    for (auto& child : m_childs) {
        child->update_absopacity(m_absopacity);
    }
}
// ----------------------------------------------------------------------------
uint32_t Widget::apply_opacity(uint32_t color) const {
    return (color & 0xffffff) | ((uint32_t)((color >> 24) * m_absopacity) << 24);
}
// ----------------------------------------------------------------------------
void Widget::set_clipping(bool clipping) {
    m_clipping = clipping;
    dirty();
}
// ----------------------------------------------------------------------------
