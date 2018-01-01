#include "cat_ui_service.h"
#include "cat_kernel_api.h"
#include "cat_gfx_renderer.h"

using namespace cat;

// ----------------------------------------------------------------------------
UIService::UIService(KernelApi* kernel) {
    m_kernel = kernel;
    m_scale = 1.0;
    m_desktop = new Widget(kernel, Rect2i(0,0,0,0));
    m_desktop->set_bgcolor(0);
}
// ----------------------------------------------------------------------------
UIService::~UIService() {
    delete m_desktop;
    m_kernel = nullptr;
}
// ----------------------------------------------------------------------------
bool UIService::init() {
    return true;
}
// ----------------------------------------------------------------------------
void UIService::fini() {
    m_desktop->remove_childs();
}
// ----------------------------------------------------------------------------
void UIService::pause() {
    m_desktop->notify_pause(true);
}
// ----------------------------------------------------------------------------
void UIService::resume() {
    m_desktop->notify_pause(false);
}
// ----------------------------------------------------------------------------
void UIService::context_lost() {
    m_desktop->context_lost();
}
// ----------------------------------------------------------------------------
bool UIService::context_restored() {
    return m_desktop->context_restored();
}
// ----------------------------------------------------------------------------
void UIService::resize(int width, int height) {
    m_size.width = width;
    m_size.height = height;
    m_desktop->set_size((int)((float)m_size.width / m_scale), (int)((float)m_size.height / m_scale));
}
// ----------------------------------------------------------------------------
void UIService::scale(float factor) {
    m_kernel->renderer()->draw2d.scale(factor);
    m_scale = factor;
    m_desktop->notify_uiscaled();
    m_desktop->set_size((int)((float)m_size.width / m_scale), (int)((float)m_size.height / m_scale));
}
// ----------------------------------------------------------------------------
bool UIService::touch(TouchEvent ev) {
    ev.x = (int)(ev.x / m_scale);
    ev.y = (int)(ev.y / m_scale);
    return m_desktop->touch(ev, false);
}
// ----------------------------------------------------------------------------
void UIService::render(Renderer* renderer, Timestamp now) {
    renderer->draw2d.scale(m_scale);
    m_desktop->render(renderer, now);
}
// ----------------------------------------------------------------------------
void UIService::capture(Texture* tex, const Rect2i& rect) {
    Rect2i screen_rect;
    screen_rect.origin.x = (int)(rect.origin.x * m_scale);
    screen_rect.origin.y = (int)(m_size.height - (rect.origin.y + rect.size.height) * m_scale);
    screen_rect.size.width = (int)(rect.size.width * m_scale);
    screen_rect.size.height = (int)(rect.size.height * m_scale);
    tex->capture_screen(screen_rect);
}
// ----------------------------------------------------------------------------
