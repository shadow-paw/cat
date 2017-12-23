#include "testpane.h"

using namespace app;
using namespace cat;

// ----------------------------------------------------------------------------
TestPane::TestPane(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Pane(kernel_api, rect, id) {
    // use custom shader to draw pane background
    m_shader = draw2d()->retain_2dshader(kernel_api->res(), "/assets/shader/custom.shader");

    const char* ui_image = "/assets/ui/default_ui.png";
    auto button = new Button(kernel_api, Rect2i((rect.size.width - 120) / 2, (rect.size.height - 40) / 2, 120, 40), 3);
    button->set_text("Check Button");
    button->set_bgcolor(0xffffffff);
    button->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button->set_texture(Button::TexChecked, ui_image, 94, 40, 186, 78, 6, 6);
    button->set_checkable(true);
    attach(button);

    auto edit = new Editbox(kernel_api, Rect2i(10, 10, 120, 40), 1);
    edit->set_bgcolor(0x30000000);
    edit->set_text("edit1");
    edit->set_textsize(10);
    attach(edit);

    button->ev_click += [](Widget* w) -> bool {
        Logger::d("App", "TestPane Button Clicked!");
        return true;
    };
}
// ----------------------------------------------------------------------------
TestPane::~TestPane() {
    draw2d()->release_2dshader(kernel()->res(), m_shader);
}
// ----------------------------------------------------------------------------
void TestPane::cb_context_lost() {
    m_capture_tex.release();
    m_tex[0].release();
    m_tex[1].release();
}
// ----------------------------------------------------------------------------
bool TestPane::cb_context_restored() {
    m_tex[0].update(Texture::RGB, m_absrect.size.width/8, m_absrect.size.height/8, nullptr, true);
    m_tex[1].update(Texture::RGB, m_absrect.size.width/8, m_absrect.size.height/8, nullptr, true);
    return true;
}
// ----------------------------------------------------------------------------
void TestPane::cb_resize() {
    m_tex[0].update(Texture::RGB, m_absrect.size.width/8, m_absrect.size.height/8, nullptr, true);
    m_tex[1].update(Texture::RGB, m_absrect.size.width/8, m_absrect.size.height/8, nullptr, true);
}
// ----------------------------------------------------------------------------
void TestPane::cb_render(Renderer* r, Timestamp now) {
    // capture screen within this widget region
    capture(&m_capture_tex, m_absrect);
    // use the captured screen to draw effect
    Rect2i rc(0, 0, m_absrect.size.width/8, m_absrect.size.height/8);
    r->draw2d.target(&m_tex[0]);
    r->draw2d.fill(rc, 0xffffffff, &m_capture_tex, now);
    r->draw2d.target(&m_tex[1]);
    r->draw2d.fill(rc, 0xffffffff, &m_tex[0], now, m_shader);
    r->draw2d.target(nullptr);
    r->draw2d.fill(m_absrect, 0xffffffff, &m_tex[1], now);
    Pane::cb_render(r, now);    // default draw e.g. background color

    r->dirty();
}
// ----------------------------------------------------------------------------
