#include "cat_ui_effectview.h"
#include "cat_gfx_renderer.h"
#include "cat_util_log.h"

using namespace cat;

// ----------------------------------------------------------------------------
EffectView::EffectView(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Widget(kernel_api, rect, id) {
    m_texrefs.resize(1);
    m_effect = Draw2D::Effect::Color;
    m_detail_level = 8;
    m_texsize.width = m_texsize.height = 0;
}
// ----------------------------------------------------------------------------
EffectView::~EffectView() {
}
// ----------------------------------------------------------------------------
void EffectView::cb_context_lost() {
    m_tex[0].release();
    m_tex[1].release();
    m_tex[2].release();
}
// ----------------------------------------------------------------------------
bool EffectView::cb_context_restored() {
    resize_tex();
    return true;
}
// ----------------------------------------------------------------------------
void EffectView::cb_resize() {
    resize_tex();
}
// ----------------------------------------------------------------------------
void EffectView::resize_tex() {
    if (!kernel()->renderer()->ready()) return;
    Size2i s;
    s.width = m_absrect.size.width/m_detail_level;
    s.height = m_absrect.size.height/m_detail_level;
    if (m_texsize == s) return;
    m_texsize = s;
    m_tex[1].update(Texture::RGB, m_texsize.width, m_texsize.height, nullptr, false);
    m_tex[2].update(Texture::RGB, m_texsize.width, m_texsize.height, nullptr, true);
}
// ----------------------------------------------------------------------------
void EffectView::set_effect(Draw2D::Effect effect, int detail_level) {
    m_effect = effect;
    if (m_detail_level != detail_level) {
        m_detail_level = detail_level;
        resize_tex();
    }
    dirty();
}
// ----------------------------------------------------------------------------
void EffectView::cb_render(Renderer* r, Timestamp now) {
    switch (m_effect) {
    case Draw2D::Effect::Color:
        r->draw2d.fill(m_absrect, apply_opacity(m_bgcolor));
        break;
    case Draw2D::Effect::Tex:
        r->draw2d.fill(m_absrect, apply_opacity(m_bgcolor), m_texrefs[TexBackground], now);
        break;
    default: {
            Rect2i rc(0, 0, m_absrect.size.width/m_detail_level, m_absrect.size.height/m_detail_level);
            capture(&m_tex[0], m_absrect);                      // m_tex[0] := raw screen background
            r->draw2d.target(&m_tex[1]);
            r->draw2d.fill(rc, 0xffffffff, &m_tex[0], now);   // m_tex[1] := scaled down screen background
            r->draw2d.target(&m_tex[2]);
            r->draw2d.fill(rc, 0xffffffff, &m_tex[1], now, m_effect);   // m_tex[2] = effect
            r->draw2d.target(nullptr);
            r->draw2d.fill(m_absrect, apply_opacity(m_bgcolor), &m_tex[2], now);  // render on screen
            r->dirty();
        }
    }
}
// ----------------------------------------------------------------------------
