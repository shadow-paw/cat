#ifndef __OSAL_STORAGE_DRIVER_H__
#define __OSAL_STORAGE_DRIVER_H__

#include <string>
#include "osal_storage_buffer.h"

namespace osal {
// ----------------------------------------------------------------------------
class Driver {
public:
    Driver() = default;
    virtual ~Driver() = default;
    virtual bool read(const std::string& name, Buffer& buffer) = 0;
    virtual bool write(const std::string& name, const Buffer& buffer) = 0;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_STORAGE_DRIVER_H__
