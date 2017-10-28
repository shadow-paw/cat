#include "cat_util_buffer.h"
#include <stdlib.h>

using namespace cat;

// ----------------------------------------------------------------------------
Buffer::Buffer() {
    m_buffer = nullptr;
    m_size = 0;
}
// ----------------------------------------------------------------------------
Buffer::~Buffer() {
    release();
}
// ----------------------------------------------------------------------------
Buffer::Buffer(Buffer&& o) {
    m_buffer = o.m_buffer;
    m_size = o.m_size;
    o.m_buffer = nullptr;
    o.m_size = 0;
}
// ----------------------------------------------------------------------------
Buffer::Buffer(const std::string& o) {
    m_buffer = nullptr;
    m_size = 0;
    alloc(o.size() + 1);
    memcpy(m_buffer, o.c_str(), o.size() + 1);
}
// ----------------------------------------------------------------------------
Buffer& Buffer::operator=(Buffer&& o) {
    m_buffer = o.m_buffer;
    m_size = o.m_size;
    o.m_buffer = nullptr;
    o.m_size = 0;
    return *this;
}
// ----------------------------------------------------------------------------
Buffer& Buffer::operator=(const std::string& o) {
    alloc(o.size()+1);
    memcpy(m_buffer, o.c_str(), o.size()+1);
    return *this;
}
// ----------------------------------------------------------------------------
bool Buffer::alloc(size_t size) {
    uint8_t* p = (uint8_t*)realloc(m_buffer, size);
    if (!p) return false;
    m_buffer = p;
    m_size = size;
    return true;
}
// ----------------------------------------------------------------------------
void Buffer::release() {
    if (m_buffer) {
        free(m_buffer);
        m_buffer = nullptr;
    } m_size = 0;
}
// ----------------------------------------------------------------------------
void Buffer::shrink(size_t s) {
    if (m_size > s) m_size = s;
}
// ----------------------------------------------------------------------------
