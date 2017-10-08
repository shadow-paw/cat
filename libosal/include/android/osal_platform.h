#ifndef __OSAL_PLATFORM_ANDROID_H__
#define __OSAL_PLATFORM_ANDROID_H__
#if defined(PLATFORM_ANDROID)

#include <jni.h>
#include <GLES2/gl2.h>
#include "osal_jni.h"
#include "osal_storage_driver_asset.h"

namespace osal {
// ----------------------------------------------------------------------------
struct PlatformSpecificData {
    jobject rootview;
    jobject asset_manager;
};
// ----------------------------------------------------------------------------
} // namespace osal:

#endif // PLATFORM_ANDROID
#endif // __OSAL_PLATFORM_ANDROID_H__
