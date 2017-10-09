#include "app.h"

using namespace app;
using namespace osal;

// ----------------------------------------------------------------------------
AppKernel::AppKernel() {
}
// ----------------------------------------------------------------------------
AppKernel::~AppKernel() {
}
// ----------------------------------------------------------------------------
bool AppKernel::cb_startup(Timestamp now) {
    storage::Buffer buf;
    if (vfs()->read("/assets/test.txt", buf)) {
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
    const gfx::Shader* shader = res()->retain_shader("/assets/shader/test", uniforms, attrs);
    util::Logger::d("app", "shader: %p", shader);
    res()->release_shader(shader);
    return true;
}
// ----------------------------------------------------------------------------
void AppKernel::cb_shutdown(Timestamp now) {
}
// ----------------------------------------------------------------------------
bool AppKernel::cb_context_lost() {
    return true;
}
// ----------------------------------------------------------------------------
void AppKernel::cb_context_restored() {
}
// ----------------------------------------------------------------------------
void AppKernel::cb_resize(int width, int height) {
}
// ----------------------------------------------------------------------------
void AppKernel::cb_render(gfx::Renderer* r, Timestamp now) {
    gfx::Rect2i rect;
    gfx::TextStyle style;
    rect.set(10, 10, 100, 40);
    style.color = 0xffff00ff;
    r->draw2d.drawtext(rect, "Hello", style);
}
// ----------------------------------------------------------------------------
