#ifndef __CAT_STORAGE_DRIVER_FILE_H__
#define __CAT_STORAGE_DRIVER_FILE_H__

#include <string>
#include "cat_storage_driver.h"

namespace cat {
// ----------------------------------------------------------------------------
class FileDriver: public Driver {
public:
    FileDriver(const std::string& base);
    virtual ~FileDriver();
    virtual bool read(const std::string& name, Buffer* buffer);
    virtual bool write(const std::string& name, const Buffer& buffer);
private:
    std::string m_base;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_STORAGE_DRIVER_FILE_H__
