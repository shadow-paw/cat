#ifndef __CAT_GFX_DRAWABLECANVAS_H__
#define __CAT_GFX_DRAWABLECANVAS_H__

#include <stdint.h>
#include "cat_platform.h"
#include "cat_type.h"
#include "cat_gfx_type.h"
#include "cat_gfx_tex.h"

#if defined(PLATFORM_MAC)
  #include <ApplicationServices/ApplicationServices.h>
#elif defined(PLATFORM_IOS)
  #include <CoreGraphics/CoreGraphics.h>
  #include <CoreText/CoreText.h>
#endif

namespace cat {
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
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    HDC       m_hdc;
    HBITMAP   m_hbitmap, m_obitmap;
    HFONT     m_hfont, m_ofont;
    void*     m_bmpixel;
    int       m_width, m_height;
    uint32_t* m_pixel;
    static COLORREF rgba2gdicolor(uint32_t color);
    void     update_rgba();

#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    int   m_width, m_height;
    uint32_t* m_pixel;
    CGContextRef m_context;
    CTFontRef    m_font;
    CGColorRef   m_textcolor;
    CFAttributedStringRef create_string(const char* utf8); 
#elif defined(PLATFORM_ANDROID)
    int       m_width, m_height;
    struct {
        jclass bitmap_class, canvas_class, paint_class, rect_class, glut_class;
        jmethodID bitmap_eraseColor,
            canvas_drawText, canvas_drawRect, canvas_drawLine,
            paint_setTextSize, paint_setARGB, paint_getTextBounds,
            rect_width, rect_height,
            glut_texImage2D;
        jobject bitmap, canvas, paint, rect;
    } m_jni; 
#else
    #error Not Implemented!
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_GFX_DRAWABLECANVAS_H__
