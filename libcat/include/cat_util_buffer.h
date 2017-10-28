#ifndef __CAT_UTIL_BUFFER_H__
#define __CAT_UTIL_BUFFER_H__

#include <stdint.h>
#include <stddef.h>
#include <string>

namespace cat {
// ----------------------------------------------------------------------------
class Buffer {
public:
    Buffer();
    Buffer(const void* p, size_t size);
    Buffer(Buffer&& o);
    ~Buffer();

    uint8_t* ptr() const { return m_buffer; }
    size_t   size() const { return m_size; }
    bool alloc(size_t size);
    void release();
    void shrink(size_t s);

    Buffer& operator=(Buffer&& o);
    Buffer& operator=(const std::string& o);
    operator uint8_t*() const { return m_buffer; }

private:
    uint8_t* m_buffer;
    size_t   m_size;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UTIL_BUFFER_H__
