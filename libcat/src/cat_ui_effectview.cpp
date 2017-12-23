#include "cat_ui_effectview.h"
#include "cat_gfx_renderer.h"

using namespace cat;

// ----------------------------------------------------------------------------
EffectView::EffectView(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Widget(kernel_api, rect, id) {
    m_texrefs.resize(1);
    m_effect = Draw2D::Effect::Color;
    m_detail_level = 8;
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
    m_tex[1].update(Texture::RGB, m_absrect.size.width/m_detail_level, m_absrect.size.height/m_detail_level, nullptr, false);
    m_tex[2].update(Texture::RGB, m_absrect.size.width/m_detail_level, m_absrect.size.height/m_detail_level, nullptr, true);
    return true;
}
// ----------------------------------------------------------------------------
void EffectView::cb_resize() {
    m_tex[1].update(Texture::RGB, m_absrect.size.width/m_detail_level, m_absrect.size.height/m_detail_level, nullptr, false);
    m_tex[2].update(Texture::RGB, m_absrect.size.width/m_detail_level, m_absrect.size.height/m_detail_level, nullptr, true);
}
// ----------------------------------------------------------------------------
void EffectView::set_effect(Draw2D::Effect effect, int detail_level) {
    m_effect = effect;
    if (m_detail_level != detail_level) {
        m_detail_level = detail_level;
        m_tex[1].update(Texture::RGB, m_absrect.size.width/m_detail_level, m_absrect.size.height/m_detail_level, nullptr, false);
        m_tex[2].update(Texture::RGB, m_absrect.size.width/m_detail_level, m_absrect.size.height/m_detail_level, nullptr, true);
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
            r->draw2d.fill(rc, 0xffffffff, &m_tex[0], now);     // m_tex[1] := scaled down screen background
            r->draw2d.target(&m_tex[2]);
            r->draw2d.fill(rc, 0xffffffff, &m_tex[0], now, m_effect);   // m_tex[2] = effect
            r->draw2d.target(nullptr);
            r->draw2d.fill(m_absrect, apply_opacity(m_bgcolor), &m_tex[2], now);  // render on screen
            r->dirty();
        }
    }
}
// ----------------------------------------------------------------------------
