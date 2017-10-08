#ifndef __OSAL_GFX_CANVAS_H__
#define __OSAL_GFX_CANVAS_H__

// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
  #include "win/osal_gfx_canvas.h"
#elif defined(PLATFORM_MAC)
  #include "mac/osal_gfx_canvas.h"
#elif defined(PLATFORM_IOS)
  #include "ios/osal_gfx_canvas.h"
#elif defined(PLATFORM_ANDROID)
  #include "android/osal_gfx_canvas.h"
#else
  #error Not Implemented!
#endif
// ----------------------------------------------------------------------------

#endif // __OSAL_GFX_CANVAS_H__
