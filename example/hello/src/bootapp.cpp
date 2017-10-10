#include "bootapp.h"

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
bool BootApp::cb_startup(time::Timestamp now) {
    storage::Buffer buf;
    if (kernel()->vfs()->read("/assets/test.txt", buf)) {
        util::Logger::d("app", (const char*)buf.data());
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
    const gfx::Shader* shader = kernel()->res()->retain_shader("/assets/shader/test", uniforms, attrs);
    util::Logger::d("app", "shader: %p", shader);
    kernel()->res()->release_shader(shader);
    
    auto widget = new ui::Label(kernel(), gfx::Rect2i(10, 10, 100, 100));
    widget->set_bgcolor(0xffff00ff);
    widget->set_text("hello");
    kernel()->ui()->attach(widget);

    kernel()->time()->post_timer(this, 1000, 1);
    return true;
}
// // cb_shutdown is called after app->exit()
// ----------------------------------------------------------------------------
void BootApp::cb_shutdown(time::Timestamp now) {
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
}
// cb_render is called in the render pipeline
// ----------------------------------------------------------------------------
void BootApp::cb_render(gfx::Renderer* r, time::Timestamp now) {
    gfx::Rect2i rect;
    gfx::TextStyle style;
    rect.set(10, 10, 100, 40);
    style.color = 0xffff00ff;
    r->draw2d.drawtext(rect, "Hello", style);
}
// ----------------------------------------------------------------------------
bool BootApp::cb_timer(time::Timestamp now, int msg) {
    util::Logger::d("app", "timer: %lu", now);
    kernel()->time()->post_timer(this, 1000, 1);
    return true;
}
// ----------------------------------------------------------------------------

