#include "cat_data_buffer.h"
#include <stdlib.h>

using namespace cat;

// ----------------------------------------------------------------------------
Buffer::Buffer() {
    m_buffer = nullptr;
    m_size = m_allocated = 0;
}
// ----------------------------------------------------------------------------
Buffer::~Buffer() {
    free();
}
// ----------------------------------------------------------------------------
Buffer::Buffer(Buffer&& o) {
    m_buffer = o.m_buffer;
    m_size = o.m_size;
    m_allocated = o.m_allocated;
    o.m_buffer = nullptr;
    o.m_size = 0;
    o.m_allocated = 0;
}
// ----------------------------------------------------------------------------
Buffer::Buffer(const void* p, size_t size) {
    m_buffer = nullptr;
    m_size = 0;
    m_allocated = 0;
    realloc(size);
    memcpy(m_buffer, p, size);
}
// ----------------------------------------------------------------------------
Buffer& Buffer::operator=(Buffer&& o) {
    m_buffer = o.m_buffer;
    m_size = o.m_size;
    m_allocated = o.m_allocated;
    o.m_buffer = nullptr;
    o.m_size = 0;
    o.m_allocated = 0;
    return *this;
}
// ----------------------------------------------------------------------------
Buffer& Buffer::operator=(const std::string& o) {
    realloc(o.size()+1);
    memcpy(m_buffer, o.c_str(), o.size()+1);
    return *this;
}
// ----------------------------------------------------------------------------
bool Buffer::realloc(size_t size) {
    if (m_allocated < size) {
        uint8_t* p = (uint8_t*)::realloc(m_buffer, size);
        if (!p) return false;
        m_buffer = p;
        m_allocated = size;
    }
    m_size = size;
    return true;
}
// ----------------------------------------------------------------------------
void Buffer::free() {
    if (m_buffer) {
        ::free(m_buffer);
        m_buffer = nullptr;
    }
    m_size = 0;
    m_allocated = 0;
}
// ----------------------------------------------------------------------------
void Buffer::shrink(size_t s) {
    if (m_size > s) m_size = s;
}
// ----------------------------------------------------------------------------
