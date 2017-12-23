#ifndef __CAT_UI_SERVICE_H__
#define __CAT_UI_SERVICE_H__

#include "cat_platform.h"
#include "cat_type.h"
#include "cat_time_type.h"
#include "cat_ui_widget.h"
#include "cat_gfx_renderer.h"
#include "cat_kernel_api.h"

namespace cat {
class Kernel;
// ----------------------------------------------------------------------------
class UIService {
friend class cat::Kernel;
public:
    UIService(KernelApi* kernel);
    ~UIService();

    bool attach(Widget* w);
    void detach(Widget* w);
    void scale(float factor);
    float get_scale() const { return m_scale; }
    int get_width() const { return m_width; }
    int get_height() const { return m_height; }
    void capture(Texture* tex, const Rect2i& rect);

private:    // call from Kernel
    void context_lost();
    bool context_restored();
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
} // namespace cat

#endif // __CAT_UI_SERVICE_H__
