#ifndef __OSAL_PLATFORM_IOS_H__
#define __OSAL_PLATFORM_IOS_H__
#if defined(PLATFORM_IOS)

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

namespace osal {
// ----------------------------------------------------------------------------
struct PlatformSpecificData {
    void* rootview;
};
// ----------------------------------------------------------------------------
} // namespace osal:

#endif // PLATFORM_IOS
#endif // __OSAL_PLATFORM_IOS_H__
