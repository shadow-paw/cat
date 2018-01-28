#include "cat_storage_driver_file.h"
#include <stdio.h>

using namespace cat;

// ----------------------------------------------------------------------------
FileDriver::FileDriver(const std::string& base, unsigned int flags) {
    // force tail /
    m_base = base.back() == '/' ? base : base + "/";
    m_flags = flags;
}
// ----------------------------------------------------------------------------
FileDriver::~FileDriver() {
}
// ----------------------------------------------------------------------------
bool FileDriver::read(const std::string& name, Buffer* buffer) {
    if (name.find("../") != std::string::npos) return false;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (name.find("..\\") != std::string::npos) return false;
    FILE* fp = nullptr;
    if (fopen_s(&fp, (m_base + name).c_str(), "rb")) return false;
    fseek(fp, 0, SEEK_END);
    off_t length = (off_t)_ftelli64(fp);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
    FILE* fp = nullptr;
    if ((fp = fopen((m_base + name).c_str(), "rb")) == nullptr) return false;
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
    if ((m_flags & FLAG_WRITABLE) != FLAG_WRITABLE) return false;
    if (name.find("../") != std::string::npos) return false;
    FILE *fp = nullptr;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (name.find("..\\") != std::string::npos) return false;
    if (fopen_s(&fp, (m_base + name).c_str(), "wb")) return false;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
    if ((fp = fopen((m_base + name).c_str(), "rb")) == nullptr) return false;
#else
    #error Not Implemented!
#endif
    if (fwrite(buffer.ptr(), buffer.size(), 1, fp) != 1) goto fail;
    fclose(fp);
    return true;
fail:
    fclose(fp);
    return false;
}
// ----------------------------------------------------------------------------
