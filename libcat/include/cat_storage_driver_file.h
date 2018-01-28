#ifndef __CAT_STORAGE_DRIVER_FILE_H__
#define __CAT_STORAGE_DRIVER_FILE_H__

#include <string>
#include "cat_storage_driver.h"

namespace cat {
// ----------------------------------------------------------------------------
class FileDriver: public Driver {
public:
    static const unsigned int FLAG_READONLY = 0;
    static const unsigned int FLAG_WRITABLE = 1 << 0;

    FileDriver(const std::string& base, unsigned int flags);
    virtual ~FileDriver();
    virtual bool read(const std::string& name, Buffer* buffer);
    virtual bool write(const std::string& name, const Buffer& buffer);
private:
    std::string m_base;
    bool m_flags;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_STORAGE_DRIVER_FILE_H__
