#ifndef __OSAL_GFX_CANVAS_MAC_H__
#define __OSAL_GFX_CANVAS_MAC_H__

#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>
#include <stdint.h>
#include "osal_platform.h"
#include "osal_type.h"
#include "osal_gfx_type.h"
#include "osal_gfx_tex.h"

namespace osal {
// ----------------------------------------------------------------------------
class DrawableCanvas {
public:
    DrawableCanvas();
    ~DrawableCanvas();
    bool resize(int w, int h);
    void release();
    void texImage2D(Texture& tex);
    void clear(unsigned int color);
    void set_textstyle(const TextStyle& style);
    void set_textcolor(unsigned int color);
    void calctext(const char* utf8, int* w, int* h);
    void drawtext(const char* utf8, int x, int y, int w, int h);
    void line(unsigned int color, int x1, int y1, int x2, int y2);
    void fill(unsigned int color, int x1, int y1, int x2, int y2);

private:
    int   m_width, m_height;
    uint32_t* m_pixel;
    CGContextRef m_context;
    CTFontRef    m_font;
    CGColorRef   m_textcolor;
    CFAttributedStringRef create_string(const char* utf8);
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_GFX_CANVAS_MAC_H__
