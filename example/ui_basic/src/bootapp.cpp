#include "bootapp.h"
#include "pane1.h"
#include "pane2.h"
#include "pane3.h"

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
    auto desktop = kernel()->ui()->desktop();

    // Pane with texture
    auto pane1 = new Pane1(kernel(), Rect2i(0, 0, 0, 0));
    desktop->attach(pane1);
    auto pane3 = new Pane3(kernel(), Rect2i(0, 0, 400, 300));
    desktop->attach(pane3);

    m_pane = new Pane2(kernel(), Rect2i(0, 0, 256, 256));
    desktop->attach(m_pane);

    desktop->ev_layout += [this, pane1, pane3](Widget* w) -> void {
        pane1->set_size(w->get_size().width / 2, w->get_size().height);   // half desktop width
    };
    desktop->ev_layout.call(desktop);  // trigger relayout

    // handle UI events
    pane1->ev_check += [this](Widget* w, bool checked) -> void {
        Logger::d("App", "Pane1 Checked: %s!", checked ? "YES" : "NO");
        if (checked) {
            auto interpolator = std::shared_ptr<Interpolator>(new AccelerateInterpolator());
            m_pane->animators.translate.set_interpolator(interpolator);
            m_pane->animators.opacity.set_interpolator(interpolator);
            m_pane->animators.translate.start(Point2i(-100, 100), Point2i(0, 100));
            m_pane->animators.opacity.start(0, 1.0f);
            m_pane->animators.opacity.set_callback(nullptr);
            m_pane->set_visible(true);
        } else {
            auto interpolator = std::shared_ptr<Interpolator>(new AccelerateInterpolator());
            m_pane->animators.translate.set_interpolator(interpolator);
            m_pane->animators.opacity.set_interpolator(interpolator);
            m_pane->animators.translate.start(Point2i(0, 100), Point2i(-100, 100));
            m_pane->animators.opacity.start(1.0f, 0);
            m_pane->animators.opacity.set_callback([this]() -> void {
                m_pane->set_visible(false);
            });
        }
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
void BootApp::cb_context_lost() {
    Logger::d("App", "cb_context_lost");
}
// cb_context_restored is called when the GL context is restored
// you should reload any manual created gfx resources here.
// resources retained by resource manager will be auto reloaded by the kernel.
// ----------------------------------------------------------------------------
bool BootApp::cb_context_restored() {
    Logger::d("App", "cb_context_restored");
    return true;
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
bool BootApp::cb_button1_click(cat::Widget* widget) {
    Logger::d("App", "Button1 Clicked!");
    return true;
}
// ----------------------------------------------------------------------------
