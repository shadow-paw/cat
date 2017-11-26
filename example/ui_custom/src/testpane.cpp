#include "testpane.h"

using namespace app;
using namespace cat;

// ----------------------------------------------------------------------------
TestPane::TestPane(KernelApi* kernel, const Rect2i& rect, unsigned int id) : Pane(kernel, rect, id) {
    // use custom shader to draw pane background
    m_shader = draw2d()->retain_2dshader(kernel->res(), "/assets/shader/test");

    const char* ui_image = "/assets/ui/default_ui.png";
    auto button = new Button(kernel, Rect2i((rect.size.width - 120) / 2, (rect.size.height - 40) / 2, 120, 40), 3);
    button->set_text("Check Button");
    button->set_bgcolor(0xffffffff);
    button->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button->set_texture(Button::TexChecked, ui_image, 94, 40, 186, 78, 6, 6);
    button->set_checkable(true);
    attach(button);

    auto edit = new Editbox(kernel, Rect2i(10, 10, 120, 40), 1);
    edit->set_bgcolor(0x30000000);
    edit->set_text("edit1");
    edit->set_textsize(10);
    attach(edit);

    button->ev_click = [](Widget* w) -> bool {
        Logger::d("App", "TestPane Button Clicked!");
        return true;
    };
}
// ----------------------------------------------------------------------------
TestPane::~TestPane() {
    draw2d()->release_2dshader(kernel()->res(), m_shader);
}
// ----------------------------------------------------------------------------
void TestPane::cb_render(Renderer* r, unsigned long now) {
    // capture screen within this widget region
    capture(m_effect_tex, m_absrect);
    // use the captured screen to draw effect
    r->draw2d.fill(m_absrect, 0xffffffff, &m_effect_tex, now, m_shader);
    r->dirty();
    Pane::cb_render(r, now);    // default draw e.g. background color
}
// ----------------------------------------------------------------------------
