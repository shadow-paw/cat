#ifndef __CAT_GFX_VBO_H__
#define __CAT_GFX_VBO_H__

#include <stdint.h>
#include <stddef.h>
#include "cat_gfx_type.h"

namespace cat {
// ----------------------------------------------------------------------------
class VBO {
public:
    VBO();
    ~VBO();
    bool  init(size_t size, bool dynamic);
    void  fini();
    bool  resize(size_t size);
    void* lock();
    void  unlock();

    const void* get()    const { return m_data; }
    size_t      size()   const { return m_size; }
    void        bind()   const { glBindBuffer(GL_ARRAY_BUFFER, m_vbo); }
    void        unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

private:
    GLuint m_vbo;
    void*  m_data;
    size_t m_size;
    GLenum m_type;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_GFX_VBO_H__
