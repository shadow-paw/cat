#ifndef __OSAL_KERNEL_API_H__
#define __OSAL_KERNEL_API_H__

#include "osal_platform.h"
#include "osal_type.h"

namespace osal {
// ----------------------------------------------------------------------------
class Renderer;
class ResourceManager;
class VFS;
class TimeService;
class UIService;
class Application;
// ----------------------------------------------------------------------------
class KernelApi {
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
    virtual bool run(Application* app) = 0;

    virtual const PlatformSpecificData* psd() = 0;
    virtual VFS*             vfs() = 0;
    virtual Renderer*        renderer() = 0;
    virtual ResourceManager* res() = 0;
    virtual TimeService*     time() = 0;
    virtual UIService*       ui() = 0;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_KERNEL_API_H__
