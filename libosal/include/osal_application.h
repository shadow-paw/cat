#ifndef __OSAL_APPLICATION_H__
#define __OSAL_APPLICATION_H__

#include "osal_platform.h"
#include "osal_type.h"
#include "osal_storage_vfs.h"
#include "osal_gfx_renderer.h"
#include "osal_gfx_resmgr.h"
#include "osal_kernel_api.h"

namespace osal {
// ----------------------------------------------------------------------------
class Application : public TimerHandler<int> {
friend class Kernel;
public:
    Application() {
        m_kernel = nullptr;
        m_running = false;
    }
    virtual ~Application() {
        kernel()->time()->remove_timer(this);
    }
protected:
    virtual bool cb_startup(Timestamp now) = 0;
    virtual void cb_shutdown(Timestamp now) = 0;
    virtual void cb_pause() = 0;
    virtual void cb_resume() = 0;
    virtual bool cb_context_lost() = 0;
    virtual void cb_context_restored() = 0;
    virtual void cb_resize(int width, int height) = 0;
    virtual void cb_render(Renderer* r, Timestamp now) = 0;
    virtual bool cb_timer(Timestamp now, int msg) = 0;
protected:
    KernelApi* kernel() const { return m_kernel; }
    void exit() { m_running = false; }
private:
    KernelApi* m_kernel;
    bool m_running;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_APPLICATION_H__
