#include "cat_storage_driver_asset.h"

#if defined(PLATFORM_ANDROID)
#include <stdio.h>
#include <android/asset_manager_jni.h>

using namespace cat;

// ----------------------------------------------------------------------------
AssetDriver::AssetDriver(jobject asset_manager) {
    JNIHelper jni;
    m_manager = AAssetManager_fromJava(jni.env(), asset_manager);
}
// ----------------------------------------------------------------------------
AssetDriver::~AssetDriver() {
    m_manager = nullptr;
}
// ----------------------------------------------------------------------------
bool AssetDriver::read(const std::string& name, Buffer* buffer) {
    AAsset* asset = AAssetManager_open(m_manager, name.c_str(), AASSET_MODE_STREAMING);
    if (!asset) return false;
    off_t length = AAsset_getLength64(asset);
    if (!buffer->realloc((size_t)length + 1)) goto fail;
    AAsset_read(asset, buffer->ptr(), (size_t)length);
    AAsset_close(asset);
    buffer->ptr()[length] = 0; // zero pad
    buffer->realloc((size_t)length);
    return true;
fail:
    AAsset_close(asset);
    return false;
}
// ----------------------------------------------------------------------------
bool AssetDriver::write(const std::string& name, const Buffer& buffer) {
    // TODO: Not implemented yet
    return false;    
}
// ----------------------------------------------------------------------------
#endif // PLATFORM_ANDROID
