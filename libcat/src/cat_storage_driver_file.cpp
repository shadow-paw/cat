#include "cat_storage_driver_file.h"
#include <stdio.h>

using namespace cat;

// ----------------------------------------------------------------------------
FileDriver::FileDriver(const std::string& base) {
    // force tail /
    m_base = base.back() == '/' ? base : base + "/";
}
// ----------------------------------------------------------------------------
FileDriver::~FileDriver() {
}
// ----------------------------------------------------------------------------
bool FileDriver::read(const std::string& name, Buffer* buffer) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    FILE* fp = nullptr;
    if (fopen_s(&fp, (m_base + name).c_str(), "rb")) return false;
    fseek(fp, 0, SEEK_END);
    off_t length = (off_t)_ftelli64(fp);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
    FILE* fp = fopen((m_base + name).c_str(), "rb");
    if (!fp) return false;
    fseek(fp, 0, SEEK_END);
    off_t length = ftello(fp);
#else
    #error Not Implemented!
#endif
    fseek(fp, 0, SEEK_SET);
    if (!buffer->realloc((size_t)length + 1)) goto fail;
    if (fread(buffer->ptr(), (size_t)length, 1, fp) != 1) goto fail;
    fclose(fp);
    buffer->ptr()[length] = 0; // zero pad
    buffer->realloc((size_t)length);
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
