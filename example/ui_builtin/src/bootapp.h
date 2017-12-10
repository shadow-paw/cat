#ifndef __APP_BOOTAPP_H__
#define __APP_BOOTAPP_H__

#include "libcat.h"

namespace app {
// ----------------------------------------------------------------------------
class BootApp : public cat::Application {
public:
    BootApp();
    virtual ~BootApp();
private:
    bool cb_button1_click(cat::Widget* widget);
private:
    cat::Pane* m_pane;
protected:
    virtual bool cb_startup(cat::Timestamp now);
    virtual void cb_shutdown(cat::Timestamp now);
    virtual void cb_pause();
    virtual void cb_resume();
    virtual bool cb_context_lost();
    virtual void cb_context_restored();
    virtual void cb_resize(int width, int height);
    virtual void cb_render(cat::Renderer* r, cat::Timestamp now);
    virtual bool cb_timer(cat::Timestamp now, int msg);
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_KERNEL_H__
