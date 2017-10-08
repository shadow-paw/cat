#ifndef __OSAL_PLATFORM_H__
#define __OSAL_PLATFORM_H__

// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
  #include "win/osal_platform.h"
#elif defined(PLATFORM_MAC)
  #include "mac/osal_platform.h"
#elif defined(PLATFORM_IOS)
  #include "ios/osal_platform.h"
#elif defined(PLATFORM_ANDROID)
  #include "android/osal_platform.h"
#else
    #error Not Implemented!
#endif

namespace osal {
// ----------------------------------------------------------------------------
enum Platform {
    Windows,
    Mac,
    IOS,
    Android
};
// ----------------------------------------------------------------------------
} // namespace osal:

#endif // __OSAL_PLATFORM_H__
