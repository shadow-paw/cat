#include <stdlib.h>
#include "osal_gfx_canvas.h"

using namespace osal;

// ----------------------------------------------------------------------------
DrawableCanvas::DrawableCanvas() {
    m_jni.bitmap_class = nullptr;
    m_jni.canvas_class = nullptr;
    m_jni.paint_class = nullptr;
    m_jni.rect_class = nullptr;
    m_jni.glut_class = nullptr;
    m_jni.bitmap = nullptr;
    m_jni.canvas = nullptr;
    m_jni.paint = nullptr;
    m_jni.rect = nullptr;
}
// ----------------------------------------------------------------------------
DrawableCanvas::~DrawableCanvas() {
    release();
}
// ----------------------------------------------------------------------------
bool DrawableCanvas::resize(int width, int height) {
    m_width = width;
    m_height = height;
    JNIHelper jni;
    // jcfg = Bitmap.Config.valueOf(ARGB_8888)
    jclass    jcfg_class = (jclass)jni.env()->FindClass("android/graphics/Bitmap$Config");
    jmethodID jcfg_valueOf = jni.env()->GetStaticMethodID(jcfg_class, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject jcfg = jni.env()->CallStaticObjectMethod(jcfg_class, jcfg_valueOf, jni.env()->NewStringUTF("ARGB_8888"));
    // jbitmap = Bitmap.createBitmap(w, h, jcfg);
    jclass    jbitmap_class = (jclass)jni.env()->FindClass("android/graphics/Bitmap");
    jmethodID jbitmap_ctor = jni.env()->GetStaticMethodID(jbitmap_class, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject   jbitmap = jni.env()->CallStaticObjectMethod(jbitmap_class, jbitmap_ctor, width, height, jcfg);
    // jcanvas = new Canvas();
    jclass    jcanvas_class = (jclass)jni.env()->FindClass("android/graphics/Canvas");
    jmethodID jcanvas_ctor = jni.env()->GetMethodID(jcanvas_class, "<init>", "(Landroid/graphics/Bitmap;)V");
    jobject   jcanvas = jni.env()->NewObject(jcanvas_class, jcanvas_ctor, jbitmap);
    // jpaint = new Paint(ANTI_ALIAS_FLAG);
    const jint ANTI_ALIAS_FLAG = 1;
    jclass    jpaint_class = (jclass)jni.env()->FindClass("android/graphics/Paint");
    jmethodID jpaint_ctor = jni.env()->GetMethodID(jpaint_class, "<init>", "(I)V");
    jobject   jpaint = jni.env()->NewObject(jpaint_class, jpaint_ctor, ANTI_ALIAS_FLAG);
    // Rect
    jclass    jrect_class = (jclass)jni.env()->FindClass("android/graphics/Rect");
    jmethodID jrect_ctor = jni.env()->GetMethodID(jrect_class, "<init>", "()V");
    jobject   jrect = jni.env()->NewObject(jrect_class, jrect_ctor);
    // GLUT
    jclass glut_class = (jclass)jni.env()->FindClass("android/opengl/GLUtils");

    // Get Method
    m_jni.bitmap_class = (jclass)jni.env()->NewGlobalRef(jbitmap_class);
    m_jni.bitmap_eraseColor = jni.env()->GetMethodID(jbitmap_class, "eraseColor", "(I)V");
    m_jni.canvas_class = (jclass)jni.env()->NewGlobalRef(jcanvas_class);
    m_jni.canvas_drawText = jni.env()->GetMethodID(jcanvas_class, "drawText", "(Ljava/lang/String;FFLandroid/graphics/Paint;)V");
    m_jni.canvas_drawRect = jni.env()->GetMethodID(jcanvas_class, "drawRect", "(FFFFLandroid/graphics/Paint;)V");
    m_jni.canvas_drawLine = jni.env()->GetMethodID(jcanvas_class, "drawLine", "(FFFFLandroid/graphics/Paint;)V");
    m_jni.paint_class = (jclass)jni.env()->NewGlobalRef(jpaint_class);
    m_jni.paint_setTextSize = jni.env()->GetMethodID(jpaint_class, "setTextSize", "(F)V");
    m_jni.paint_setARGB = jni.env()->GetMethodID(jpaint_class, "setARGB", "(IIII)V");
    m_jni.paint_getTextBounds = jni.env()->GetMethodID(jpaint_class, "getTextBounds", "(Ljava/lang/String;IILandroid/graphics/Rect;)V");
    m_jni.rect_class = (jclass)jni.env()->NewGlobalRef(jrect_class);
    m_jni.rect_height = jni.env()->GetMethodID(jrect_class, "height", "()I");
    m_jni.rect_width = jni.env()->GetMethodID(jrect_class, "width", "()I");
    m_jni.glut_class = (jclass)jni.env()->NewGlobalRef(glut_class);
    m_jni.glut_texImage2D = jni.env()->GetStaticMethodID(glut_class, "texImage2D", "(IILandroid/graphics/Bitmap;I)V");

    // Retain objects
    m_jni.bitmap = jni.env()->NewGlobalRef(jbitmap);
    m_jni.canvas = jni.env()->NewGlobalRef(jcanvas);
    m_jni.paint = jni.env()->NewGlobalRef(jpaint);
    m_jni.rect = jni.env()->NewGlobalRef(jrect);

    jni.env()->CallVoidMethod(m_jni.paint, m_jni.paint_setARGB, 0, 0, 0, 0);
    return false;
fail:
    release();
    return false;
}
// ----------------------------------------------------------------------------
void DrawableCanvas::release() {
    JNIHelper jni;
    if (m_jni.bitmap_class) {
        jni.env()->DeleteGlobalRef(m_jni.bitmap_class);
        m_jni.bitmap_class = nullptr;
    }
    if (m_jni.canvas_class) {
        jni.env()->DeleteGlobalRef(m_jni.canvas_class);
        m_jni.canvas_class = nullptr;
    }
    if (m_jni.rect_class) {
        jni.env()->DeleteGlobalRef(m_jni.rect_class);
        m_jni.rect_class = nullptr;
    }
    if (m_jni.glut_class) {
        jni.env()->DeleteGlobalRef(m_jni.glut_class);
        m_jni.glut_class = nullptr;
    }
    if (m_jni.bitmap) {
        jni.env()->DeleteGlobalRef(m_jni.bitmap);
        m_jni.bitmap = nullptr;
    }
    if (m_jni.canvas) {
        jni.env()->DeleteGlobalRef(m_jni.canvas);
        m_jni.canvas = nullptr;
    }
    if (m_jni.paint) {
        jni.env()->DeleteGlobalRef(m_jni.paint);
        m_jni.paint = nullptr;
    }
    if (m_jni.rect) {
        jni.env()->DeleteGlobalRef(m_jni.rect);
        m_jni.rect = nullptr;
    }
}
// ----------------------------------------------------------------------------
void DrawableCanvas::texImage2D(Texture& tex) {
    JNIHelper jni;
    tex.bind(0);
    // GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
    jni.env()->CallStaticVoidMethod(m_jni.glut_class, m_jni.glut_texImage2D, GL_TEXTURE_2D, 0, m_jni.bitmap, 0);
    tex.unbind(0);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::clear(unsigned int color) {
    JNIHelper jni;
    jni.env()->CallVoidMethod(m_jni.bitmap, m_jni.bitmap_eraseColor, color);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::set_textcolor(unsigned int color) {
    JNIHelper jni;
    jni.env()->CallVoidMethod(m_jni.paint, m_jni.paint_setARGB,
        color >> 24, 0xFF & (color >> 16), 0xFF & (color >> 8), 0xFF & color);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::set_textstyle(const TextStyle& style) {
    JNIHelper jni;
    jni.env()->CallVoidMethod(m_jni.paint, m_jni.paint_setTextSize, (float)style.fontsize);
    // TODO: Bold with Typeface.DEFAULT_BOLD
}
// ----------------------------------------------------------------------------
void DrawableCanvas::calctext(const char* utf8, int* w, int* h) {
    JNIHelper jni;
    jstring s = jni.env()->NewStringUTF(utf8);
    jsize slen = jni.env()->GetStringLength(s);
    jni.env()->CallVoidMethod(m_jni.paint, m_jni.paint_getTextBounds, s, 0, slen, m_jni.rect);
    *w = jni.env()->CallIntMethod(m_jni.rect, m_jni.rect_width) + 1; // reserve +1 for shadow
    *h = jni.env()->CallIntMethod(m_jni.rect, m_jni.rect_height) + 1;
}
// ----------------------------------------------------------------------------
void DrawableCanvas::drawtext(const char* utf8, int x, int y, int w, int h) {
    JNIHelper jni;
    jstring s = jni.env()->NewStringUTF(utf8);
    jni.env()->CallVoidMethod(m_jni.canvas, m_jni.canvas_drawText, s, (float)x, (float)(m_height - y - 2), m_jni.paint);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::line(unsigned int color, int x1, int y1, int x2, int y2) {
    JNIHelper jni;
    jni.env()->CallVoidMethod(m_jni.canvas, m_jni.canvas_drawLine, (float)x1, (float)(m_height - y1),(float)x2, (float)(m_height - y2), m_jni.paint);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::fill(unsigned int color, int x1, int y1, int x2, int y2) {
    // TODO: Implement
}
// ----------------------------------------------------------------------------
