#ifndef __CAT_STORAGE_DRIVER_H__
#define __CAT_STORAGE_DRIVER_H__

#include <string>
#include "cat_data_buffer.h"

namespace cat {
// ----------------------------------------------------------------------------
class Driver {
public:
    Driver() = default;
    virtual ~Driver() = default;
    virtual bool read(const std::string& name, Buffer* buffer) = 0;
    virtual bool write(const std::string& name, const Buffer& buffer) = 0;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_STORAGE_DRIVER_H__
