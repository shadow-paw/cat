#include "cat_gfx_ibo.h"
#include <stdlib.h>

using namespace cat;

// ----------------------------------------------------------------------------
IBO::IBO() {
    m_ibo   = 0;
    m_data  = nullptr;
    m_count = 0;
    m_size = 0;
}
// ----------------------------------------------------------------------------
IBO::~IBO() {
    fini();
}
// ----------------------------------------------------------------------------
IBO::IBO(IBO&& o) {
    m_ibo = o.m_ibo;     o.m_ibo = 0;
    m_data = o.m_data;   o.m_data = nullptr;
    m_count = o.m_count; o.m_count = 0;
    m_size = o.m_size;   o.m_size = 0;
}
// ----------------------------------------------------------------------------
IBO& IBO::operator=(IBO&& o) {
    m_ibo = o.m_ibo;     o.m_ibo = 0;
    m_data = o.m_data;   o.m_data = nullptr;
    m_count = o.m_count; o.m_count = 0;
    m_size = o.m_size;   o.m_size = 0;
    return *this;
}
// ----------------------------------------------------------------------------
void IBO::fini() {
    if (m_ibo)  { glDeleteBuffers (1, &m_ibo ); m_ibo = 0; }
    if (m_data) { free(m_data); m_data = nullptr; }
    m_count = 0;
    m_size = 0;
}
// ----------------------------------------------------------------------------
bool IBO::init(size_t count) {
    return resize(count);
}
// ----------------------------------------------------------------------------
bool IBO::resize(size_t count) {
    if (count) {
        size_t size  = count * sizeof(uint16_t);
        size  = ( size + 7 ) & 0xFFFFFFF8; // align to 8 bytes
        uint16_t* p = (uint16_t*) realloc(m_data, size);
        if (!p) return false;
        m_data = p;
        m_count = count;
        m_size = size;
    } else {
        fini();
    } return true;
}
// ----------------------------------------------------------------------------
uint16_t* IBO::lock() {
    return m_data;
}
// ----------------------------------------------------------------------------
void IBO::unlock() {
    if (m_data) {
        if ( m_ibo == 0 ) glGenBuffers( 1, &m_ibo );
        bind();
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr )m_size, m_data, GL_STATIC_DRAW );
        unbind();
    }
}
// ----------------------------------------------------------------------------
