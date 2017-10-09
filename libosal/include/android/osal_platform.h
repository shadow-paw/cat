#ifndef __OSAL_PLATFORM_ANDROID_H__
#define __OSAL_PLATFORM_ANDROID_H__

#include <jni.h>
#include <GLES2/gl2.h>
#include "osal_jni.h"
#include "osal_storage_driver_asset.h"

namespace osal {
// ----------------------------------------------------------------------------
struct PlatformSpecificData {
    jobject rootview;
};
// ----------------------------------------------------------------------------
} // namespace osal:

#endif // __OSAL_PLATFORM_ANDROID_H__
