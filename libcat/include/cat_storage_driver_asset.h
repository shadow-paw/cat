#ifndef __CAT_STORAGE_DRIVER_ASSET_H__
#define __CAT_STORAGE_DRIVER_ASSET_H__
#if defined(PLATFORM_ANDROID)

#include <string>
#include <android/asset_manager_jni.h>
#include "cat_platform.h"
#include "cat_storage_driver.h"

namespace cat {
// ----------------------------------------------------------------------------
class AssetDriver : public Driver {
public:
    AssetDriver(jobject asset_manager);
    virtual ~AssetDriver();
    virtual bool read(const std::string& name, Buffer* buffer);
    virtual bool write(const std::string& name, const Buffer& buffer);
private:
    AAssetManager* m_manager;
};
// ----------------------------------------------------------------------------
} // namespace cat::storage

#endif // PLATFORM_ANDROID
#endif // __CAT_STORAGE_DRIVER_ASSET_H__
