#ifndef __CAT_PLATFORM_H__
#define __CAT_PLATFORM_H__

#include <string>

// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <WindowsX.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // NOTHING
#elif defined(PLATFORM_ANDROID)
    #include <jni.h>
    #include "cat_util_jni.h"
#else
    #error Not Implemented!
#endif

namespace cat {
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
#elif defined(PLATFORM_MAC)
    void* rootview; 
    std::string res_path;
#elif defined(PLATFORM_IOS)
    void* rootview;
    std::string res_path;
#elif defined(PLATFORM_ANDROID)
    jobject rootview; 
    jobject asset_manager;
#else
    #error Not Implemented!
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat:

#endif // __CAT_PLATFORM_H__
