#ifndef __CAT_GFX_OPENGL_H__
#define __CAT_GFX_OPENGL_H__

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <WindowsX.h>
    #undef WIN32_LEAN_AND_MEAN
    #include "glew/glew.h"
    #include "glew/wglew.h"
#elif defined(PLATFORM_MAC)
    #include <OpenGL/gl3.h>
#elif defined(PLATFORM_IOS)
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
#elif defined(PLATFORM_ANDROID)
    #include <GLES3/gl3.h>
#else
    #error Not Implemented!
#endif

#endif // __CAT_GFX_OPENGL_H__
