#include "osal_storage_buffer.h"
#include <stdlib.h>

using namespace osal;

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
