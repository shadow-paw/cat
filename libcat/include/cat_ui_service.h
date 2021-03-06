#ifndef __CAT_UI_SERVICE_H__
#define __CAT_UI_SERVICE_H__

#include "cat_platform.h"
#include "cat_type.h"
#include "cat_data_copyable.h"
#include "cat_time_type.h"
#include "cat_ui_widget.h"
#include "cat_gfx_renderer.h"
#include "cat_kernel_api.h"

namespace cat {
class Kernel;
// ----------------------------------------------------------------------------
class UIService : private NonCopyable<UIService> {
friend class cat::Kernel;
public:
    UIService(KernelApi* kernel);
    ~UIService();

    Widget* desktop() { return m_desktop; }
    void          scale(float factor);
    const Size2i& get_size() const { return m_size; }
    float         get_scale() const { return m_scale; }
    void          capture(Texture* tex, const Rect2i& rect);

private:    // call from Kernel
    //! Initialize service
    bool init();
    //! Cleanup service
    void fini();
    //! Called from kernel when the app is put to background
    void pause();
    //! Called from kernel when the app is resume to foreground
    void resume();

    void context_lost();
    bool context_restored();
    void resize(int width, int height);
    void render(Renderer* r, Timestamp now);
    bool touch(TouchEvent ev);

private:
    KernelApi* m_kernel;
    Size2i     m_size;
    float      m_scale;
    Widget*    m_desktop;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_SERVICE_H__
