#ifndef __CAT_GFX_RENDERER_H__
#define __CAT_GFX_RENDERER_H__

#include "cat_type.h"
#include "cat_gfx_draw2d.h"

namespace cat {
// ----------------------------------------------------------------------------
class Renderer {
public:
    Draw2D draw2d;

    Renderer();
    ~Renderer();
    bool init();
    void fini();
    void resize(int width, int height);
    void context_lost();
    bool context_restored();
    void begin_render();
    void end_render();

    int width() const { return m_width; }
    int height() const { return m_height; }
    bool is_dirty() const { return m_dirty; }
    void dirty() { m_dirty = true; }
    bool ready() const { return m_contextready; }

private:
    bool initGL();

private:
    bool m_contextready;
    bool m_dirty;
    int  m_width, m_height;
    GLuint m_vao;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_GFX_RENDERER_H__
