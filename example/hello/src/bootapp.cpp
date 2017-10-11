#include "bootapp.h"
#include "testpane.h"

using namespace app;
using namespace osal;

// ----------------------------------------------------------------------------
BootApp::BootApp() {
}
// ----------------------------------------------------------------------------
BootApp::~BootApp() {
}
// cb_startup is called upon kernel->run(app)
// ----------------------------------------------------------------------------
bool BootApp::cb_startup(Timestamp now) {
    Buffer buf;
    if (kernel()->vfs()->read("/assets/test.txt", buf)) {
        Logger::d("app", (const char*)buf.data());
    }
    std::unordered_map<int, std::string> uniforms = {
        { 1, "uScreenHalf" },
        { 2, "uTex0" },
        { 3, "uTime" }
    };
    std::unordered_map<int, std::string> attrs = {
        { 1, "inPosition" },
        { 2, "inColor"    },
        { 3, "inTexcoord" }
    };
    const Shader* shader = kernel()->res()->retain_shader("/assets/shader/test", uniforms, attrs);
    Logger::d("app", "shader: %p", shader);
    kernel()->res()->release_shader(shader);
    
    // UI test
    const char* ui_image = "/assets/ui/default_ui.png";

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
    auto slider1 = new Slider(kernel(), Rect2i(20, 70, 200, 50), 3);
    slider1->set_texture(Slider::TexBackground, ui_image, 0, 80, 92, 118, 6, 6);
    slider1->set_texture(Slider::TexThumb, ui_image, 94, 80, 139, 117, 6, 6);
    pane1->attach(slider1);
    slider1->set_max(1);
    slider1->set_pos(1);

    auto pane2 = new Pane(kernel(), Rect2i(300, 100, 400, 300));
    pane2->set_bgcolor(0x80ffffff);
    pane2->set_bgeffect(Draw2D::Effect::Blur);
    pane2->set_bounded(true);
    pane2->set_draggable(true);
    kernel()->ui()->attach(pane2);

    auto pane3 = new Pane(kernel(), Rect2i(400, 200, 500, 500));
    pane3->set_bgcolor(0x80ffffff);
    pane3->set_bgeffect(Draw2D::Effect::Ripple);
    pane3->set_bounded(true);
    pane3->set_draggable(true);
    kernel()->ui()->attach(pane3);
    auto button1 = new Button(kernel(), Rect2i(10, 10, 120, 40));
    button1->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button1->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button1->set_texture(Button::TexChecked, ui_image, 94, 40, 186, 78);
    button1->set_text("Button");
    pane3->attach(button1);
    auto button2 = new Button(kernel(), Rect2i(10, 70, 120, 40));
    button2->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button2->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button2->set_texture(Button::TexChecked, ui_image, 94, 40, 186, 78);
    button2->set_text("Check Button");
    button2->set_checkable(true);
    pane3->attach(button2);
    auto slider2 = new Slider(kernel(), Rect2i(150, 50, 50, 200), 4);
    slider2->set_texture(Slider::TexBackground, ui_image, 0, 80, 92, 118, 6, 6);
    slider2->set_texture(Slider::TexThumb, ui_image, 94, 80, 139, 117, 6, 6);
    slider2->set_orentation(Slider::Orentation::Vertical);
    pane3->attach(slider2);
    slider2->set_range(0, 2);
    slider2->set_pos(2);

    auto testpane = new TestPane(kernel(), Rect2i(350, 50, 200, 200), 4);
    testpane->set_bgcolor(0x80ffffff);
    testpane->set_draggable(true);
    testpane->set_bounded(true);
    kernel()->ui()->attach(testpane);

    button1->ev_click += [](Widget* w) -> bool {
        Logger::d("App", "Button Clicked!");
        return true;
    };
    button2->ev_check += [](Widget* w, bool checked) -> bool {
        Logger::d("App", "Button Checkek: %s!", checked?"YES":"NO");
        return true;
    };
    slider1->ev_slide += [](Widget* w, int pos) -> bool {
        Logger::d("App", "Slider1: %d", pos);
        return true;
    };
    slider2->ev_slide += [](Widget* w, int pos) -> bool {
        Logger::d("App", "Slider2: %d", pos);
        return true;
    };
    kernel()->time()->post_timer(this, 1000, 1);
    return true;
}
// // cb_shutdown is called after app->exit()
// ----------------------------------------------------------------------------
void BootApp::cb_shutdown(Timestamp now) {
}
// cb_pause is called when the program is going background
// ----------------------------------------------------------------------------
void BootApp::cb_pause() {
}
// cb_resume is called when the program has resumed
// ----------------------------------------------------------------------------
void BootApp::cb_resume() {
}
// cb_context_lost is called when the GL context is lost
// you should release any manual created gfx resources here.
// resources retained by resource manager will be auto reloaded by the kernel.
// ----------------------------------------------------------------------------
bool BootApp::cb_context_lost() {
    return true;
}
// cb_context_restored is called when the GL context is restored
// you should reload any manual created gfx resources here.
// resources retained by resource manager will be auto reloaded by the kernel.
// ----------------------------------------------------------------------------
void BootApp::cb_context_restored() {
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
    Rect2i rect;
    TextStyle style;
    rect.set(10, 10, 100, 40);
    style.color = 0xffff00ff;
    r->draw2d.drawtext(rect, "Hello", style);
}
// ----------------------------------------------------------------------------
bool BootApp::cb_timer(Timestamp now, int msg) {
    Logger::d("app", "timer: %lu", now);
    kernel()->time()->post_timer(this, 1000, 1);
    return true;
}
// ----------------------------------------------------------------------------

