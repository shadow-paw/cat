#ifndef __CAT_GFX_DRAWABLE_H__
#define __CAT_GFX_DRAWABLE_H__

#include <string>
#include "cat_platform.h"
#include "cat_data_copyable.h"
#include "cat_gfx_type.h"
#include "cat_gfx_tex.h"
#include "cat_gfx_drawablecanvas.h"

namespace cat {
// ----------------------------------------------------------------------------
class Drawable : private NonCopyable<Drawable> {
public:
    Drawable();
    virtual ~Drawable();

    bool resize(int width, int height);
    void release();
    const Texture* tex() const { return &m_tex; }
    bool update();

    void begin_draw();
    void end_draw();
    void clear(uint32_t color);
    void drawtext(const Rect2i& rect, const std::string& utf8, const TextStyle& style);
    void drawtext(const Rect2i& rect, const char* utf8, const TextStyle& style);
    void calctext(Size2i* size, const std::string& utf8, const TextStyle& style);
    void calctext(Size2i* size, const char* utf8, const TextStyle& style);
private:
    void set_textstyle(const TextStyle& style);
private:
    Texture m_tex;
    int     m_width, m_height;
    bool    m_dirty;
    TextStyle m_textstyle;
    DrawableCanvas m_canvas;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_GFX_DRAWABLE_H__
