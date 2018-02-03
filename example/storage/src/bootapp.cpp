#include "bootapp.h"
#include "nlohmann/json.hpp"

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
    kernel()->vfs()->write("/doc/test.txt", Buffer("test"));

    Buffer b;
    if (kernel()->vfs()->read("/doc/test.txt", &b)) {
        Logger::d("App", "read: %s", b.ptr());
    }

    // Preference using this excellent json library: https://github.com/nlohmann/json
    nlohmann::json pref = {
        {"key1", 1234.56},
        {"key2", true},
        {"key3", "hello world"},
        {"key4", nullptr},
        {"nested",{
            {"key", 9876}
        }},
        {"list",{1, 0, 2}},
        {"object",{
            {"currency", "USD"},
            {"value", 12.34}
        }}
    };
    Logger::d("App", "pref: %s", pref.dump(4).c_str());
    Logger::d("App", "pref[key1]: %f", pref["key1"].get<float>());
    Logger::d("App", "pref[nested][key]: %d", pref["nested"]["key"].get<int>());
    // Save to file
    kernel()->vfs()->write("/doc/pref.json", Buffer(pref.dump(4).c_str()));
    // Load it back
    Buffer pref_buffer;
    if (kernel()->vfs()->read("/doc/pref.json", &pref_buffer)) {
        auto pref2 = nlohmann::json::parse(pref_buffer.ptr());
        Logger::d("App", "pref2: %s", pref2.dump(4).c_str());
        Logger::d("App", "pref2[key1]: %f", pref2["key1"].get<float>());
        Logger::d("App", "pref2[nested][key]: %d", pref2["nested"]["key"].get<int>());
    }

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

