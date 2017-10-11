#include "osal_type.h"
#include "osal_kernel_api.h"
#include "osal_ui_service.h"
#include "osal_gfx_renderer.h"

using namespace osal;

// ----------------------------------------------------------------------------
UIService::UIService(KernelApi* kernel) {
    m_kernel = kernel;
    m_width = m_height = 0;
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
void UIService::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_desktop->set_size((int)((float)m_width / m_scale), (int)((float)m_height / m_scale));
}
// ----------------------------------------------------------------------------
void UIService::scale(float factor) {
    m_kernel->renderer()->draw2d.scale(factor);
    m_scale = factor;
    m_desktop->notify_uiscaled();
    m_desktop->set_size((int)((float)m_width / m_scale), (int)((float)m_height / m_scale));
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
bool UIService::attach(Widget* w) {
    return m_desktop->attach(w);
}
// ----------------------------------------------------------------------------
void UIService::detach(Widget* w) {
    m_desktop->detach(w);
}
// ----------------------------------------------------------------------------
void UIService::capture(Texture& tex, const Rect2i& rect) {
    Rect2i screen_rect;
    screen_rect.origin.x = (int)(rect.origin.x * m_scale);
    screen_rect.origin.y = (int)(m_height - (rect.origin.y + rect.size.height) * m_scale);
    screen_rect.size.width = (int)(rect.size.width * m_scale);
    screen_rect.size.height = (int)(rect.size.height * m_scale);
    tex.capture_screen(screen_rect);
}
// ----------------------------------------------------------------------------
