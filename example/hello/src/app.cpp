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
    if (vfs()->read("/test.txt", buf)) {
        util::Logger::d("app", (const char*)buf.data());
    }
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
    gfx:Rect2i rect;
    gfx::TextStyle style;
    rect.set(10, 10, 100, 40);
    style.color = 0xffff00ff;
    r->draw2d.drawtext(rect, "Hello", style);
}
// ----------------------------------------------------------------------------
