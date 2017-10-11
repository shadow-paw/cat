#ifndef __OSAL_STORAGE_DRIVER_FILE_H__
#define __OSAL_STORAGE_DRIVER_FILE_H__

#include <string>
#include "osal_storage_buffer.h"
#include "osal_storage_driver.h"

namespace osal {
// ----------------------------------------------------------------------------
class FileDriver: public Driver {
public:
    FileDriver(const std::string& base);
    virtual ~FileDriver();
    virtual bool read(const std::string& name, Buffer& buffer);
    virtual bool write(const std::string& name, const Buffer& buffer);
private:
    std::string m_base;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_STORAGE_DRIVER_FILE_H__
