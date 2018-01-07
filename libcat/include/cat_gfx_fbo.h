#ifndef __CAT_GFX_FBO_H__
#define __CAT_GFX_FBO_H__

#include "cat_data_copyable.h"
#include "cat_gfx_type.h"
#include "cat_gfx_tex.h"

namespace cat {
// ----------------------------------------------------------------------------
class FBO : private NonCopyable<FBO> {
public:
    FBO();
    ~FBO();
    // Move
    FBO(FBO&& o);
    FBO& operator=(FBO&& o);

    bool init();
    void fini();
    void bind(const Texture& tex);
    void unbind();

private:
    GLuint m_fbo;
    // old states
    struct {
        GLint fbo;
        GLint viewport[4];
    } m_state;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_GFX_VBO_H__
