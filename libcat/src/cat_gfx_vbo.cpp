#include "cat_gfx_vbo.h"
#include <stdlib.h>

using namespace cat;

// ----------------------------------------------------------------------------
VBO::VBO() {
    m_vbo = 0;
    m_data = nullptr;
    m_size = 0;
    m_type = GL_DYNAMIC_DRAW;
}
// ----------------------------------------------------------------------------
VBO::~VBO() {
    fini();
}
// ----------------------------------------------------------------------------
VBO::VBO(VBO&& o) {
    m_vbo = o.m_vbo;   o.m_vbo = 0;
    m_data = o.m_data; o.m_data = nullptr;
    m_size = o.m_size; o.m_size = 0;
    m_type = o.m_type; o.m_type = GL_DYNAMIC_DRAW;
}
// ----------------------------------------------------------------------------
VBO& VBO::operator=(VBO&& o) {
    m_vbo = o.m_vbo;   o.m_vbo = 0;
    m_data = o.m_data; o.m_data = nullptr;
    m_size = o.m_size; o.m_size = 0;
    m_type = o.m_type; o.m_type = GL_DYNAMIC_DRAW;
    return *this;
}
// ----------------------------------------------------------------------------
void VBO::fini() {
    if (m_data) { free(m_data); m_data = nullptr; }
    if (m_vbo)  { glDeleteBuffers (1, &m_vbo ); m_vbo = 0; }
    m_size = 0;
}
// ----------------------------------------------------------------------------
bool VBO::init(size_t size, bool dynamic) {
    m_type = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    return resize(size);
}
// ----------------------------------------------------------------------------
bool VBO::resize (size_t size) {
    if (size) {
        void* p = realloc(m_data, size);
        if (!p) return false;
        m_data = p;
        m_size = size;
    } else {
        fini();
    } return true;
}
// ----------------------------------------------------------------------------
void* VBO::lock() {
    return m_data;
}
// ----------------------------------------------------------------------------
void VBO::unlock() {
    if (m_data) {
        if (m_vbo == 0) glGenBuffers(1, &m_vbo);
        bind();
        glBufferData( GL_ARRAY_BUFFER, (GLsizeiptr )m_size, m_data, m_type );
        unbind();
    }
}
// ----------------------------------------------------------------------------
