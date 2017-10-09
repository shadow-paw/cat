#ifndef __OSAL_PLATFORM_WIN_H__
#define __OSAL_PLATFORM_WIN_H__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WindowsX.h>
#undef WIN32_LEAN_AND_MEAN
#include "glew/glew.h"

namespace osal {
// ----------------------------------------------------------------------------
struct PlatformSpecificData {
    HWND rootview;
};
// ----------------------------------------------------------------------------
} // namespace osal:

#endif // __OSAL_PLATFORM_WIN_H__
