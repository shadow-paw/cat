#ifndef __OSAL_PLATFORM_MAC_H__
#define __OSAL_PLATFORM_MAC_H__

#include <OpenGL/gl.h>
#include <GLUT/glut.h>

namespace osal {
// ----------------------------------------------------------------------------
struct PlatformSpecificData {
    void* rootview;
};
// ----------------------------------------------------------------------------
} // namespace osal:

#endif // __OSAL_PLATFORM_MAC_H__
