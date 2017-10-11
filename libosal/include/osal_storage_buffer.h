#ifndef __OSAL_STORAGE_BUFFER_H__
#define __OSAL_STORAGE_BUFFER_H__

#include <stdint.h>
#include <stddef.h>

namespace osal {
// ----------------------------------------------------------------------------
class Buffer {
public:
    Buffer();
    ~Buffer();
    uint8_t* data() const { return m_buffer; }
    size_t   size() const { return m_size; }
    bool alloc(size_t size);
    void release();

private:
    uint8_t* m_buffer;
    size_t   m_size;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_STORAGE_BUFFER_H__
