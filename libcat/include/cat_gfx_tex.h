#ifndef __CAT_GFX_TEX_H__
#define __CAT_GFX_TEX_H__

#include "cat_gfx_type.h"

namespace cat {
// ----------------------------------------------------------------------------
class Texture {
friend class FBO;
public:
    enum Format {
        RGB,
        RGBA,
    };
    Texture();
    ~Texture();
    int  width() const  { return m_width;  }
    int  height() const { return m_height; }
    void bind(unsigned int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_tex);
    }
    void unbind(unsigned int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    bool update(Texture::Format format, int width, int height, const void* pixel, bool antialias=true);
    bool capture_screen(const Rect2i& rect);
    void release();
private:
    int    m_width, m_height;
    GLuint m_tex;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_GFX_TEX_H__
