#include "bootapp.h"

using namespace app;
using namespace cat;

class Foo {
public:
    int foo;
};


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
    Observable<Foo> observable;

    auto cancellable1 = observable.subscribe([](const Foo& data) -> void {
        Logger::d("App", "observer1: foo = %d", data.foo);
    });
    observable.data().foo = 1;
    observable.notify();
    auto cancellable2 = observable.subscribe([](const Foo& data) -> void {
        Logger::d("App", "observer2: foo = %d", data.foo);
    });
    observable.data().foo = 2;
    observable.notify();
    cancellable2.cancel();
    observable.data().foo = 3;
    observable.notify();
    cancellable1.cancel();
    observable.data().foo = 4;
    observable.notify();
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
    Rect2i rect;
    TextStyle style;
    rect.set(10, 10, 100, 40);
    style.color = 0xffff00ff;
    r->draw2d.drawtext(rect, "Hello", style);
}
// ----------------------------------------------------------------------------
bool BootApp::cb_timer(Timestamp now, int msg) {
    Logger::d("App", "cb_timer");
    return true;
}
// ----------------------------------------------------------------------------

