#ifndef __OSAL_PLATFORM_H__
#define __OSAL_PLATFORM_H__

// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #include <WindowsX.h>
  #undef WIN32_LEAN_AND_MEAN
  #include "glew/glew.h"
#elif defined(PLATFORM_MAC)
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
#elif defined(PLATFORM_IOS)
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#elif defined(PLATFORM_ANDROID)
  #include <jni.h>
  #include <GLES2/gl2.h>
  #include "osal_util_jni.h"
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
struct PlatformSpecificData {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    HWND rootview;
#elif defined(PLATFORM_MAC)
    void* rootview; 
#elif defined(PLATFORM_IOS)
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
