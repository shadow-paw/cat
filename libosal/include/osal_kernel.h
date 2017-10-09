#ifndef __OSAL_KERNEL_H__
#define __OSAL_KERNEL_H__

#include <memory>
#include <list>
#include "osal_platform.h"
#include "osal_type.h"
#include "osal_storage_vfs.h"
#include "osal_gfx_renderer.h"
#include "osal_gfx_resmgr.h"
#include "osal_kernel_api.h"
#include "osal_application.h"

namespace osal {
// ----------------------------------------------------------------------------
class Kernel : public KernelAPI {
public:
    Kernel();
    virtual ~Kernel();

    bool init(const PlatformSpecificData& psd);
    void fini();
    Timestamp now() const;

    bool run(Application* app);

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
    virtual const PlatformSpecificData* psd() { return &m_psd; }
    virtual storage::VFS*               vfs() { return &m_vfs; }
    virtual gfx::Renderer*              renderer() { return &m_renderer; }
    virtual gfx::ResourceManager*       res() { return &m_res; }

private:
    PlatformSpecificData m_psd;
    storage::VFS         m_vfs;
    gfx::Renderer        m_renderer;
    gfx::ResourceManager m_res;
    std::list<std::unique_ptr<Application>> m_apps;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_KERNEL_H__
