#ifndef __OSAL_PLATFORM_MAC_H__
#define __OSAL_PLATFORM_MAC_H__
#if defined(PLATFORM_MAC)

#include <OpenGL/gl.h>
#include <GLUT/glut.h>

namespace osal {
// ----------------------------------------------------------------------------
struct PlatformSpecificData {
    void* rootview;
};
// ----------------------------------------------------------------------------
} // namespace osal:

#endif // PLATFORM_MAC
#endif // __OSAL_PLATFORM_MAC_H__
