#ifndef __OSAL_GFX_DRAWABLE_H__
#define __OSAL_GFX_DRAWABLE_H__

#include <string>
#include "osal_platform.h"
#include "osal_gfx_type.h"
#include "osal_gfx_tex.h"
#include "osal_gfx_canvas.h"

namespace osal {
// ----------------------------------------------------------------------------
class Drawable {
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
} // namespace osal

#endif // __OSAL_GFX_DRAWABLE_H__
