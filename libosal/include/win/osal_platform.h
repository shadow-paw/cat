#ifndef __OSAL_PLATFORM_WIN_H__
#define __OSAL_PLATFORM_WIN_H__
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)

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

#endif // PLATFORM_WIN32 || PLATFORM_WIN64
#endif // __OSAL_PLATFORM_WIN_H__
