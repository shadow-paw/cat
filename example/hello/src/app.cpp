#include "app.h"

using namespace app;
using namespace osal;

// ----------------------------------------------------------------------------
MyApp::MyApp() {
}
// ----------------------------------------------------------------------------
MyApp::~MyApp() {
}
// ----------------------------------------------------------------------------
bool MyApp::cb_startup(Timestamp now) {
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
    return true;
}
// ----------------------------------------------------------------------------
void MyApp::cb_shutdown(Timestamp now) {
}
// ----------------------------------------------------------------------------
void MyApp::cb_pause() {
}
// ----------------------------------------------------------------------------
void MyApp::cb_resume() {
}
// ----------------------------------------------------------------------------
bool MyApp::cb_context_lost() {
    return true;
}
// ----------------------------------------------------------------------------
void MyApp::cb_context_restored() {
}
// ----------------------------------------------------------------------------
void MyApp::cb_resize(int width, int height) {
}
// ----------------------------------------------------------------------------
void MyApp::cb_render(gfx::Renderer* r, Timestamp now) {
    gfx::Rect2i rect;
    gfx::TextStyle style;
    rect.set(10, 10, 100, 40);
    style.color = 0xffff00ff;
    r->draw2d.drawtext(rect, "Hello", style);
}
// ----------------------------------------------------------------------------
