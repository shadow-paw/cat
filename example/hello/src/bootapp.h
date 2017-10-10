#ifndef __APP_BOOTAPP_H__
#define __APP_BOOTAPP_H__

#include "libosal.h"

namespace app {
// ----------------------------------------------------------------------------
class BootApp : public osal::Application {
public:
    BootApp();
    virtual ~BootApp();
protected:
    virtual bool cb_startup(osal::time::Timestamp now);
    virtual void cb_shutdown(osal::time::Timestamp now);
    virtual void cb_pause();
    virtual void cb_resume();
    virtual bool cb_context_lost();
    virtual void cb_context_restored();
    virtual void cb_resize(int width, int height);
    virtual void cb_render(osal::gfx::Renderer* r, osal::time::Timestamp now);
    virtual bool cb_timer(osal::time::Timestamp now, int msg);
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_KERNEL_H__
