#ifndef __APP_KERNEL_H__
#define __APP_KERNEL_H__

#include "libosal.h"

namespace app {
// ----------------------------------------------------------------------------
class MyApp : public osal::Application {
public:
    MyApp();
    virtual ~MyApp();
protected:
    virtual bool cb_startup(osal::Timestamp now);
    virtual void cb_shutdown(osal::Timestamp now);
    virtual void cb_pause();
    virtual void cb_resume();
    virtual bool cb_context_lost();
    virtual void cb_context_restored();
    virtual void cb_resize(int width, int height);
    virtual void cb_render(osal::gfx::Renderer* r, osal::Timestamp now);
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_KERNEL_H__
