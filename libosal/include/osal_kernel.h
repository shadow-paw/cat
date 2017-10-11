#ifndef __OSAL_KERNEL_H__
#define __OSAL_KERNEL_H__

#include <memory>
#include <list>
#include "osal_platform.h"
#include "osal_type.h"
#include "osal_storage_vfs.h"
#include "osal_gfx_renderer.h"
#include "osal_gfx_resmgr.h"
#include "osal_time_service.h"
#include "osal_ui_service.h"
#include "osal_kernel_api.h"
#include "osal_application.h"

namespace osal {
// ----------------------------------------------------------------------------
class Kernel : public KernelApi {
public:
    Kernel();
    virtual ~Kernel();

    bool init(const PlatformSpecificData& psd);
    void fini();

public:
    // Called from glue
    void startup();
    void shutdown();
    void context_lost();
    bool context_restored();
    void pause();
    void resume();
    void resize(int width, int height);
    bool touch(TouchEvent ev);
    bool timer();
    void render();

public:
    // Kernel API
    virtual bool run(Application* app);
    virtual const PlatformSpecificData* psd() { return &m_psd; }
    virtual VFS*             vfs() { return &m_vfs; }
    virtual Renderer*        renderer() { return &m_renderer; }
    virtual ResourceManager* res() { return &m_res; }
    virtual TimeService*     time() { return &m_time; }
    virtual UIService*       ui() { return &m_ui; }

private:
    PlatformSpecificData  m_psd;
    VFS             m_vfs;
    Renderer        m_renderer;
    ResourceManager m_res;
    TimeService     m_time;
    UIService       m_ui;
    std::list<std::unique_ptr<Application>> m_apps;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_KERNEL_H__
