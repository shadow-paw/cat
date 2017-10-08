#ifndef __OSAL_STORAGE_DRIVER_ASSET_H__
#define __OSAL_STORAGE_DRIVER_ASSET_H__

#include <string>
#include <android/asset_manager_jni.h>
#include "osal_platform.h"
#include "osal_storage_buffer.h"
#include "osal_storage_driver.h"

namespace osal { namespace storage {
// ----------------------------------------------------------------------------
class AssetDriver : public Driver {
public:
    AssetDriver(jobject asset_manager);
    virtual ~AssetDriver();
    virtual bool read(const std::string& name, Buffer& buffer);
    virtual bool write(const std::string& name, const Buffer& buffer);
private:
    AAssetManager* m_manager;
};
// ----------------------------------------------------------------------------
}} // namespace osal::storage

#endif // __OSAL_STORAGE_DRIVER_ASSET_H__
