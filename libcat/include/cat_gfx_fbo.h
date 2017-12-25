#ifndef __CAT_GFX_FBO_H__
#define __CAT_GFX_FBO_H__

#include "cat_gfx_type.h"
#include "cat_gfx_tex.h"

namespace cat {
// ----------------------------------------------------------------------------
class FBO {
public:
    FBO();
    ~FBO();
    bool init();
    void fini();
    void bind(const Texture& tex);
    void unbind();

private:
    GLuint m_fbo;
    GLint m_viewport[4];
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_GFX_VBO_H__
