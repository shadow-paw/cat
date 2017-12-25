#ifndef __CAT_KERNEL_H__
#define __CAT_KERNEL_H__

#include <memory>
#include <list>
#include <mutex>
#include "cat_platform.h"
#include "cat_type.h"
#include "cat_gfx_renderer.h"
#include "cat_storage_vfs.h"
#include "cat_storage_resmgr.h"
#include "cat_time_service.h"
#include "cat_net_service.h"
#include "cat_ui_service.h"
#include "cat_kernel_api.h"
#include "cat_application.h"

namespace cat {
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
    virtual NetService*      net() { return &m_net; }
    virtual UIService*       ui() { return &m_ui; }

private:
    bool m_resumed;
    std::mutex      m_bigkernellock;
    PlatformSpecificData  m_psd;
    VFS             m_vfs;
    Renderer        m_renderer;
    ResourceManager m_res;
    TimeService     m_time;
    NetService      m_net;
    UIService       m_ui;
    std::list<std::unique_ptr<Application>> m_apps;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_KERNEL_H__
