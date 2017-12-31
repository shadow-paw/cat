#include "pane2.h"

using namespace app;
using namespace cat;

// ----------------------------------------------------------------------------
Pane2::Pane2(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Pane(kernel_api, rect, id) {
    const char* ui_image = "/assets/ui/default_ui.png";
    // Pane properties
    this->set_bgcolor(0x20ffffff);
    this->set_bounded(true);
    this->set_draggable(true);
    // Childs
    auto effectview = new EffectView(kernel(), Rect2i(0, 0, 256, 256));
    effectview->set_effect(Draw2D::Effect::Blur, 2);
    this->attach(effectview);

    auto button1 = new Button(kernel(), Rect2i(10, 60, 120, 40));
    button1->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button1->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button1->set_text("Button");
    this->attach(button1);

    // relayout
    this->ev_layout += [effectview, button1](Widget* w) -> void {
        effectview->set_size(w->get_size());   // fill parent
        button1->set_origin((w->get_size().width - button1->get_size().width) / 2, (w->get_size().height - button1->get_size().height) / 2);
    };
    // Events
    button1->ev_click += [](Widget* w) -> void {
        Logger::d("App", "Pane2::button1 Clicked!");
    };
}
// ----------------------------------------------------------------------------
Pane2::~Pane2() {
}
// ----------------------------------------------------------------------------
