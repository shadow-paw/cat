#ifndef __OSAL_STORAGE_VFS_H__
#define __OSAL_STORAGE_VFS_H__

#include <string>
#include <memory>
#include <unordered_map>
#include "osal_storage_buffer.h"
#include "osal_storage_driver.h"

namespace osal {
// ----------------------------------------------------------------------------
class VFS {
public:
    VFS();
    ~VFS();
    bool mount(const std::string& path, Driver* driver);
    bool unmount(const std::string& path);
    bool read(const std::string& name, Buffer& buffer);
    bool write(const std::string& name, const Buffer& buffer);

private:
    std::unordered_map<std::string,std::unique_ptr<Driver>> m_mounts;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_STORAGE_VFS_H__
