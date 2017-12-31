#include "pane1.h"

using namespace app;
using namespace cat;

// ----------------------------------------------------------------------------
Pane1::Pane1(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Pane(kernel_api, rect, id) {
    const char* ui_image = "/assets/ui/default_ui.png";
    // Pane properties
    this->set_texture(Pane::TexBackground, ui_image, 0, 0, 92, 38, 6, 6);
    this->set_bounded(true);
    this->set_draggable(true);
    // Childs
    auto label = new Label(kernel(), Rect2i(350, 10, 100, 40));
    label->set_bgcolor(0x40000000);
    label->set_text("hello world");
    label->set_textsize(20);
    label->set_textcolor(0xff9966ff);
    label->set_textgravity(TextStyle::Gravity::Center);
    label->set_clipping(true);
    this->attach(label);
    auto button1 = new Button(kernel(), Rect2i(10, 60, 120, 40));
    button1->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button1->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button1->set_text("Button");
    this->attach(button1);
    auto button2 = new Button(kernel(), Rect2i(150, 60, 120, 40));
    button2->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button2->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button2->set_texture(Button::TexChecked, ui_image, 94, 40, 186, 78, 6, 6);
    button2->set_text("Check Button");
    button2->set_checkable(true);
    this->attach(button2);
    auto slider1 = new Slider(kernel(), Rect2i(20, 110, 200, 30), 3);
    slider1->set_texture(Slider::TexBackground, ui_image, 0, 80, 92, 118, 6, 6);
    slider1->set_texture(Slider::TexThumb, ui_image, 94, 80, 139, 117, 6, 6);
    slider1->set_max(5);    // 0~5
    slider1->set_value(1);
    this->attach(slider1);
    auto edit = new Editbox(kernel(), Rect2i(10, 150, 80, 30), 1);
    edit->set_bgcolor(0x30000000);
    edit->set_text("edit1");
    edit->set_textsize(10);
    this->attach(edit);    // NOTE: editbox will always be topmost

    auto slider2 = new Slider(kernel(), Rect2i(150, 50, 50, 200), 4);
    slider2->set_texture(Slider::TexBackground, ui_image, 0, 80, 92, 118, 6, 6);
    slider2->set_texture(Slider::TexThumb, ui_image, 94, 80, 139, 117, 6, 6);
    slider2->set_orentation(Slider::Orentation::Vertical);
    slider2->set_range(0, 10);
    slider2->set_value(2);
    this->attach(slider2);

    // relayout
    this->ev_resize += [label, button1, button2, slider1, slider2](Widget* w) -> void {
        label->set_origin(0, 0);
        label->set_size(w->parent()->get_size().width/2, 40);
        button1->set_origin(0, label->get_origin().y + label->get_size().height + 4);
        button2->set_origin(button1->get_origin().x + button1->get_size().width + 16, button1->get_origin().y);
        slider1->set_origin(0, button1->get_origin().y + button1->get_size().height + 4);
        slider2->set_origin(button2->get_origin().x + button2->get_size().width + 16, button2->get_origin().y);
    };
    // Events
    button1->ev_click += [](Widget* w) -> void {
        Logger::d("App", "Pane1::button1 Clicked!");
    };
    button2->ev_check += [this](Widget* w, bool checked) -> void {
        Logger::d("App", "Pane1::button2 Check: %s", (checked?"YES":"NO"));
        ev_check.call(this, checked);
    };
    slider1->ev_slide += [](Widget* w, int pos) -> void {
        Logger::d("App", "Pane1::slider: %d", pos);
    };
}
// ----------------------------------------------------------------------------
Pane1::~Pane1() {
}
// ----------------------------------------------------------------------------
