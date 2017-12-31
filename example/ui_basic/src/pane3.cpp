#include "pane3.h"

using namespace app;
using namespace cat;

// ----------------------------------------------------------------------------
Pane3::Pane3(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Pane(kernel_api, rect, id) {
    // use custom shader to draw pane background
    m_shader = draw2d()->retain_2dshader(kernel_api->res(), "/assets/shader/custom.shader");
    m_detail_level = 4;
    // Pane properties
    this->set_bgcolor(0x20ffffff);
    this->set_bounded(true);
    this->set_draggable(true);
    // Childs
    auto label = new Label(kernel(), Rect2i(10, 60, 120, 40));
    label->set_bgcolor(0x40000000);
    label->set_textsize(16);
    label->set_textcolor(0xff9966ff);
    label->set_textgravity(TextStyle::Gravity::Center);
    label->set_clipping(true);
    label->set_text("Pane3");
    this->attach(label);

    // relayout
    this->ev_layout += [label](Widget* w) -> void {
        // center in parent
        label->set_origin((w->get_size().width - label->get_size().width) / 2, (w->get_size().height - label->get_size().height) / 2);
    };
}
// ----------------------------------------------------------------------------
Pane3::~Pane3() {
    draw2d()->release_2dshader(kernel()->res(), m_shader);
}
// ----------------------------------------------------------------------------
void Pane3::resize_tex() {
    Size2i s;
    s.width = m_absrect.size.width / m_detail_level;
    s.height = m_absrect.size.height / m_detail_level;
    if (m_texsize == s) return;
    m_texsize = s;
    m_tex[1].update(Texture::RGB, m_texsize.width, m_texsize.height, nullptr, false);
    m_tex[2].update(Texture::RGB, m_texsize.width, m_texsize.height, nullptr, true);
}
// ----------------------------------------------------------------------------
void Pane3::cb_context_lost() {
    m_tex[0].release();
    m_tex[1].release();
    m_tex[2].release();
}
// ----------------------------------------------------------------------------
bool Pane3::cb_context_restored() {
    resize_tex();
    return true;
}
// ----------------------------------------------------------------------------
void Pane3::cb_resize() {
    resize_tex();
}
// ----------------------------------------------------------------------------
void Pane3::cb_render(Renderer* r, Timestamp now) {
    // capture screen within this widget region
    capture(&m_tex[0], m_absrect);                      // m_tex[0] := raw screen background
    // use the captured screen to draw effect
    Rect2i rc(0, 0, m_absrect.size.width / m_detail_level, m_absrect.size.height / m_detail_level);
    r->draw2d.target(&m_tex[1]);
    r->draw2d.fill(rc, 0xffffffff, &m_tex[0], now);   // m_tex[1] := scaled down screen background
    r->draw2d.target(&m_tex[2]);
    r->draw2d.fill(rc, 0xffffffff, &m_tex[1], now, m_shader);   // m_tex[2] = effect
    r->draw2d.target(nullptr);
    r->draw2d.fill(m_absrect, 0xffffffff, &m_tex[2], now);  // render on screen
    r->dirty();

    Pane::cb_render(r, now);    // default draw e.g. background color
}
// ----------------------------------------------------------------------------
