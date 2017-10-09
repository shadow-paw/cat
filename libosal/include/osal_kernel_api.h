#ifndef __OSAL_KERNEL_API_H__
#define __OSAL_KERNEL_API_H__

#include "osal_platform.h"
#include "osal_type.h"
#include "osal_storage_vfs.h"
#include "osal_gfx_renderer.h"
#include "osal_gfx_resmgr.h"

namespace osal {
// ----------------------------------------------------------------------------
class KernelAPI {
public:
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

public:
    virtual const PlatformSpecificData* psd() = 0;
    virtual storage::VFS*               vfs() = 0;
    virtual gfx::Renderer*              renderer() = 0;
    virtual gfx::ResourceManager*       res() = 0;

};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_KERNEL_API_H__
