#include "osal_storage_driver_file.h"
#include <stdio.h>

using namespace osal;

// ----------------------------------------------------------------------------
FileDriver::FileDriver(const std::string& base) {
    // force tail /
    m_base = base.back() == '/' ? base : base + "/";
}
// ----------------------------------------------------------------------------
FileDriver::~FileDriver() {
}
// ----------------------------------------------------------------------------
bool FileDriver::read(const std::string& name, Buffer& buffer) {
    FILE* fp = fopen((m_base + name).c_str(), "rb");
    if (!fp) return false;
    fseek(fp, 0, SEEK_END);
    off_t length = ftello(fp);
    fseek(fp, 0, SEEK_SET);
    if (!buffer.alloc((size_t)length + 1)) goto fail;
    if (fread(buffer.data(), (size_t)length, 1, fp) != 1) goto fail;
    fclose(fp);
    buffer.data()[length] = 0; // zero pad
    return true;
fail:
    fclose(fp);
    return false;
}
// ----------------------------------------------------------------------------
bool FileDriver::write(const std::string& name, const Buffer& buffer) {
    // TODO: Not implemented yet
    return false;    
}
// ----------------------------------------------------------------------------
