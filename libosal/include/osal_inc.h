#ifndef __OSAL_INC_H__
#define __OSAL_INC_H__

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #include <WindowsX.h>
  #undef WIN32_LEAN_AND_MEAN
  #include "glew/glew.h"

#elif defined(PLATFORM_MACOSX)
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>

#elif defined(PLATFORM_IOS)
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>

#elif defined(PLATFORM_ANDROID)
  #include <jni.h>
  #include <GLES2/gl2.h>

#else
  #error Please fined a platform: PLATFORM_WIN32 PLATFORM_WIN64 PLATFORM_MACOSX PLATFORM_IOS PLATFORM_ANDROID
#endif

#endif // __OSAL_INC_H__
