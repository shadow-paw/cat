#ifndef __OSAL_UI_SERVICE_H__
#define __OSAL_UI_SERVICE_H__

#include "osal_platform.h"
#include "osal_type.h"
#include "osal_time_type.h"
#include "osal_ui_widget.h"
#include "osal_gfx_renderer.h"
#include "osal_kernel_api.h"

namespace osal {
class Kernel;
// ----------------------------------------------------------------------------
class UIService {
friend class osal::Kernel;
public:
    UIService(KernelApi* kernel);
    ~UIService();

    bool attach(Widget* w);
    void detach(Widget* w);
    void scale(float factor);
    void capture(Texture& tex, const Rect2i& rect);

private:    // call from Kernel
    void render(Renderer* r, Timestamp now);
    void resize(int width, int height);
    bool touch(TouchEvent ev);

private:
    KernelApi* m_kernel;
    int m_width, m_height;
    float m_scale;
    Widget* m_desktop;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_UI_SERVICE_H__
