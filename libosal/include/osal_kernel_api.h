#ifndef __OSAL_KERNEL_API_H__
#define __OSAL_KERNEL_API_H__

#include "osal_platform.h"
#include "osal_type.h"

// Forward declaration, sadly c++ does not allowg short class gfx::Renderer;
// ----------------------------------------------------------------------------
namespace osal { namespace gfx {
    class Renderer;
    class ResourceManager;
}}
namespace osal { namespace time {
    class TimeService;
}}
namespace osal { namespace storage {
    class VFS;
}}
namespace osal { namespace ui {
    class UIService;
}}

namespace osal {
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
    virtual const PlatformSpecificData* psd() = 0;
    virtual storage::VFS*               vfs() = 0;
    virtual gfx::Renderer*              renderer() = 0;
    virtual gfx::ResourceManager*       res() = 0;
    virtual time::TimeService*          time() = 0;
    virtual ui::UIService*              ui() = 0;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_KERNEL_API_H__
