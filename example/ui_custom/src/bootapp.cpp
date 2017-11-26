#include "bootapp.h"
#include "testpane.h"

using namespace app;
using namespace cat;

// ----------------------------------------------------------------------------
BootApp::BootApp() {
}
// ----------------------------------------------------------------------------
BootApp::~BootApp() {
}
// ----------------------------------------------------------------------------
// App Lifecycle: cb_startup -> cb_resume <-> cb_pause -> cb_shutdown
// ----------------------------------------------------------------------------
// cb_startup is called upon kernel->run(app)
// ----------------------------------------------------------------------------
bool BootApp::cb_startup(Timestamp now) {
    Logger::d("App", "cb_startup");
    const char* ui_image = "/assets/ui/default_ui.png";

    // Pane with texture
    auto pane1 = new Pane(kernel(), Rect2i(10, 10, 400, 300));
    pane1->set_texture(Pane::TexBackground, ui_image, 0, 0, 92, 38, 6, 6);
    pane1->set_bounded(true);
    pane1->set_draggable(true);
    kernel()->ui()->attach(pane1);
    auto label = new Label(kernel(), Rect2i(10, 10, 100, 40));
    label->set_bgcolor(0x40000000);
    label->set_text("hello");
    label->set_textsize(20);
    label->set_textcolor(0xff9966ff);
    pane1->attach(label);
    auto button1 = new Button(kernel(), Rect2i(10, 60, 120, 40));
    button1->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button1->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button1->set_text("Button");
    pane1->attach(button1);
    auto button2 = new Button(kernel(), Rect2i(150, 60, 120, 40));
    button2->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button2->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button2->set_texture(Button::TexChecked, ui_image, 94, 40, 186, 78, 6, 6);
    button2->set_text("Check Button");
    button2->set_checkable(true);
    pane1->attach(button2);
    auto slider1 = new Slider(kernel(), Rect2i(20, 110, 200, 30), 3);
    slider1->set_texture(Slider::TexBackground, ui_image, 0, 80, 92, 118, 6, 6);
    slider1->set_texture(Slider::TexThumb, ui_image, 94, 80, 139, 117, 6, 6);
    pane1->attach(slider1);
    slider1->set_max(5);    // 0~5
    slider1->set_pos(1);
    auto edit = new Editbox(kernel(), Rect2i(10, 150, 120, 30), 1);
    edit->set_bgcolor(0x30000000);
    edit->set_text("edit1");
    edit->set_textsize(10);
    pane1->attach(edit);    // NOTE: editbox will always be topmost

    auto testpane = new TestPane(kernel(), Rect2i(350, 50, 400, 400), 4);
    testpane->set_bgcolor(0x80ffffff);
    testpane->set_draggable(true);
    testpane->set_bounded(true);
    kernel()->ui()->attach(testpane);

    // handle UI events
    button1->ev_click = [](Widget* w) -> bool {
        Logger::d("App", "Button Clicked!");
        return true;
    };
    button2->ev_check = [](Widget* w, bool checked) -> bool {
        Logger::d("App", "Button Checked: %s!", checked ? "YES" : "NO");
        return true;
    };
    slider1->ev_slide = [](Widget* w, int pos) -> bool {
        Logger::d("App", "Slider1: %d", pos);
        return true;
    };
    return true;
}
// cb_resume is called when the program has resumed
// ----------------------------------------------------------------------------
void BootApp::cb_resume() {
    Logger::d("App", "cb_resume");
}
// cb_pause is called when the program is going background
// ----------------------------------------------------------------------------
void BootApp::cb_pause() {
    Logger::d("App", "cb_pause");
}
// cb_shutdown is called after app->exit()
// ----------------------------------------------------------------------------
void BootApp::cb_shutdown(Timestamp now) {
    Logger::d("App", "cb_shutdown");
}
// ----------------------------------------------------------------------------
// OpenGL Context. Everything retained from ResourceManager is managed and
//                 automatically restored upon cb_context_restored()
//                 You only need to handle your own resources here.
// ----------------------------------------------------------------------------
// cb_context_lost is called when the GL context is lost
// you should release any manual created gfx resources here.
// resources retained by resource manager will be auto reloaded by the kernel.
// ----------------------------------------------------------------------------
bool BootApp::cb_context_lost() {
    Logger::d("App", "cb_context_lost");
    return true;
}
// cb_context_restored is called when the GL context is restored
// you should reload any manual created gfx resources here.
// resources retained by resource manager will be auto reloaded by the kernel.
// ----------------------------------------------------------------------------
void BootApp::cb_context_restored() {
    Logger::d("App", "cb_context_restored");
}
// cb_resize is called when the screen is resized, you may adjust ui scale here
// ----------------------------------------------------------------------------
void BootApp::cb_resize(int width, int height) {
    int preferredW, preferredH;
    switch (kernel()->platform()) {
    case Platform::Windows: preferredW = 1280; preferredH = 720; break;
    case Platform::Mac:     preferredW = 1280; preferredH = 720; break;
    case Platform::IOS:     preferredW = 512;  preferredH = 960; break;
    case Platform::Android: preferredW = 512;  preferredH = 960; break;
    default:                preferredW = 512;  preferredH = 960;
    }
    float scaleX = (float)width / preferredW;
    float scaleY = (float)height / preferredH;
    kernel()->ui()->scale(scaleX<scaleY ? scaleX : scaleY);
}
// cb_render is called in the render pipeline
// ----------------------------------------------------------------------------
void BootApp::cb_render(Renderer* r, Timestamp now) {
}
// ----------------------------------------------------------------------------
bool BootApp::cb_timer(Timestamp now, int msg) {
    Logger::d("App", "cb_timer");
    return true;
}
// ----------------------------------------------------------------------------
