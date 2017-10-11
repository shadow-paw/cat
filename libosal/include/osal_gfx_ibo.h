#ifndef __OSAL_GFX_IBO_H__
#define __OSAL_GFX_IBO_H__

#include <stdint.h>
#include <stddef.h>
#include "osal_platform.h"

namespace osal {
// ----------------------------------------------------------------------------
class IBO {
public:
    IBO();
    ~IBO();
    bool      init(size_t count);
    void      fini();
    bool      resize(size_t count);
    uint16_t* lock();
    void      unlock();

    const uint16_t* get()   const { return m_data; }
    size_t          count() const { return m_count; }
    void bind()   const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo); }
    void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

private:
    GLuint m_ibo;
    uint16_t* m_data;
    size_t m_count, m_size;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_GFX_IBO_H__
