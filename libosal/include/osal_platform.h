#ifndef __OSAL_PLATFORM_H__
#define __OSAL_PLATFORM_H__

#include "osal_inc.h"

namespace osal {
// ----------------------------------------------------------------------------
enum Platform {
    Windows,
    Mac,
    IOS,
    Android
};
// ----------------------------------------------------------------------------
struct PlatformSpecificData {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    HWND rootview;
#elif defined(PLATFORM_MACOSX) || defined(PLATFORM_IOS)
	void* rootview;
#elif defined(PLATFORM_ANDROID)
    jobject rootview;
#else
    #error Not Implemented!
#endif
};
// ----------------------------------------------------------------------------
} // namespace osal:

#endif // __OSAL_PLATFORM_H__
