#ifndef __OSAL_GFX_CANVAS_ANDROID_H__
#define __OSAL_GFX_CANVAS_ANDROID_H__

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
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_GFX_CANVAS_ANDROID_H__
