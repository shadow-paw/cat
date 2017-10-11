#include "osal_storage_vfs.h"
#include <stdlib.h>

using namespace osal;

// ----------------------------------------------------------------------------
VFS::VFS() {
}
// ----------------------------------------------------------------------------
VFS::~VFS() {
}
// ----------------------------------------------------------------------------
bool VFS::mount(const std::string& path, Driver* driver) {
    // force tail /
    auto pair = m_mounts.emplace(path.back() == '/' ? path : path + "/", std::unique_ptr<Driver>(driver));
    return  pair.second;
}
// ----------------------------------------------------------------------------
bool VFS::unmount(const std::string& path) {
    return  m_mounts.erase(path) > 0;
}
// ----------------------------------------------------------------------------
bool VFS::read(const std::string& name, Buffer& buffer) {
    for (auto& mount : m_mounts) {
        if (name.find(mount.first)!=0) continue;
        if (mount.second->read(name.substr(mount.first.length()), buffer)) return true;
    } return false;
}
// ----------------------------------------------------------------------------
bool VFS::write(const std::string& name, const Buffer& buffer) {
    for (auto& mount : m_mounts) {
        if (name.find(mount.first) != 0) continue;
        if (mount.second->write(name.substr(mount.first.length()), buffer)) return true;
    } return false;
}
// ----------------------------------------------------------------------------
