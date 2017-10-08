#ifndef __OSAL_KERNEL_H__
#define __OSAL_KERNEL_H__

#include "osal_platform.h"
#include "osal_type.h"
#include "osal_gfx_renderer.h"
#include "osal_storage_vfs.h"

namespace osal {
// ----------------------------------------------------------------------------
class Kernel {
public:
    Kernel();
    virtual ~Kernel();

    bool init(const PlatformSpecificData& psd);
    void fini();

    Platform platform() const {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
        return Platform::Windows;
#elif defined(PLATFORM_MAC)
        return Platform::Mac;
#elif defined(PLATFORM_IOS)
        return Platform::IOS;
#elif defined(PLATFORM_ANDROID)
        return Platform::Android;
#else
  #error Not Implemented!
#endif
    }
    Timestamp now() const;

public:
    const PlatformSpecificData* psd() { return &m_psd; }
    gfx::Renderer*              renderer() { return &m_renderer; }
    storage::VFS*               vfs() { return &m_vfs; }

public:
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

protected:
    virtual bool cb_startup(Timestamp now) = 0;
    virtual void cb_shutdown(Timestamp now) = 0;
    virtual bool cb_context_lost() = 0;
    virtual void cb_context_restored() = 0;
    virtual void cb_resize(int width, int height) = 0;
    virtual void cb_render(gfx::Renderer* r, Timestamp now) = 0;

private:
    PlatformSpecificData m_psd;
    gfx::Renderer        m_renderer;
    storage::VFS         m_vfs;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_KERNEL_H__
