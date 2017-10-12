#if defined(PLATFORM_MAC)
  #include <ApplicationServices/ApplicationServices.h>
#endif
#include <stdlib.h>
#include "osal_gfx_canvas.h"

using namespace osal;

// ----------------------------------------------------------------------------
DrawableCanvas::DrawableCanvas() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    m_width = m_height = 0;
    m_hdc = NULL;
    m_hbitmap = m_obitmap = NULL;
    m_bmpixel = nullptr;
    m_pixel = nullptr;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    m_width = m_height = 0;
    m_pixel = nullptr;
    m_context = nullptr;
    m_font = nullptr;
    m_textcolor = nullptr;
#elif defined (PLATFORM_ANDROID)
    m_jni.bitmap_class = nullptr;
    m_jni.canvas_class = nullptr;
    m_jni.paint_class = nullptr;
    m_jni.rect_class = nullptr;
    m_jni.glut_class = nullptr;
    m_jni.bitmap = nullptr;
    m_jni.canvas = nullptr;
    m_jni.paint = nullptr;
    m_jni.rect = nullptr; 
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
DrawableCanvas::~DrawableCanvas() {
    release();
}
// ----------------------------------------------------------------------------
bool DrawableCanvas::resize(int width, int height) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    BITMAPINFO  bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;          // B,G,R,0
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;
    m_hdc = CreateCompatibleDC(NULL);
    if (m_hdc == NULL) goto fail;
    m_hbitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&m_bmpixel, NULL, 0);
    if (m_hbitmap == NULL) goto fail;
    m_obitmap = (HBITMAP)SelectObject(m_hdc, m_hbitmap);
    if (m_pixel) _aligned_free(m_pixel);
    if ((m_pixel = (uint32_t*)_aligned_malloc(width * height * 4, 32)) == NULL) goto fail;
    memset (m_pixel, 0, width * height * 4);
    SetBkColor(m_hdc, 0);
    SetBkMode(m_hdc, TRANSPARENT);
    m_width = width;
    m_height = height;
    return true;
fail:
    release();
    return false;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    uint32_t* new_pixel = (uint32_t*)realloc(m_pixel, width * height * sizeof(uint32_t));
    if (!new_pixel) return false;
    m_width = width;
    m_height = height;
    m_pixel = new_pixel;
    // Create Context
    CGColorSpaceRef cspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    m_context = CGBitmapContextCreate(m_pixel, width, height, 8, 4 * m_width, cspace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(cspace);
    // Invert transform
    CGContextSetTextMatrix(m_context, CGAffineTransformIdentity);
    CGContextTranslateCTM(m_context, 0, height);
    CGContextScaleCTM(m_context, 1.0, -1.0);
    // Clear
    set_textcolor(0xffffffff);
    clear(0);
    return true;
#elif defined (PLATFORM_ANDROID)
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
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::release() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    m_width = m_height = 0;
    if (m_hdc) {
        if (m_ofont) SelectObject(m_hdc, m_ofont);
        if (m_obitmap) SelectObject(m_hdc, m_obitmap);
        DeleteDC(m_hdc);
        m_hdc = NULL;
    }
    if (m_hfont) DeleteObject(m_hfont);
    if (m_hbitmap) DeleteObject(m_hbitmap);
    m_bmpixel = nullptr;
    if (m_pixel) {
        _aligned_free(m_pixel); m_pixel = nullptr;
    }
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    m_width = m_height = 0;
    if (m_font) { CFRelease(m_font); m_font = nullptr; }
    if (m_textcolor) { CFRelease(m_textcolor); m_textcolor = nullptr; }
    if (m_context) { CGContextRelease(m_context); m_context = nullptr; }
    if (m_pixel) { free(m_pixel); m_pixel = nullptr; }
#elif defined (PLATFORM_ANDROID)
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
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::texImage2D(Texture& tex) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    GdiFlush();
    update_rgba();
    tex.update(Texture::Format::RGBA, m_width, m_height, m_pixel);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    CGContextFlush(m_context);
    tex.bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixel);
    tex.unbind(0);
#elif defined (PLATFORM_ANDROID)
    JNIHelper jni;
    tex.bind(0);
    // GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
    jni.env()->CallStaticVoidMethod(m_jni.glut_class, m_jni.glut_texImage2D, GL_TEXTURE_2D, 0, m_jni.bitmap, 0);
    tex.unbind(0); 
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::clear(unsigned int color) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    RECT rc = { 0, 0, m_width, m_height };
    HBRUSH hBrush = CreateSolidBrush(rgba2gdicolor(color));
    FillRect(m_hdc, &rc, hBrush);
    DeleteObject(hBrush);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    if (color & 0xff000000) {
        CGFloat components[] = {
            ((CGFloat)(color & 0xFF)) / 255.0f,
            ((CGFloat)((color >> 8) & 0xFF)) / 255.0f,
            ((CGFloat)((color >> 16) & 0xFF)) / 255.0f,
            ((CGFloat)((color >> 24) & 0xFF)) / 255.0f
        };
        CGContextSetFillColor(m_context, components);
        CGContextFillRect(m_context, CGRectMake(0, 0, m_width, m_height));
    } else {
        CGContextClearRect(m_context, CGRectMake(0, 0, m_width, m_height));
    }
#elif defined (PLATFORM_ANDROID)
    JNIHelper jni;
    jni.env()->CallVoidMethod(m_jni.bitmap, m_jni.bitmap_eraseColor, color); 
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::set_textcolor(unsigned int color) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    SetTextColor(m_hdc, rgba2gdicolor(color));
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    CGFloat components[] = {
        ((CGFloat)(color & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 8) & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 16) & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 24) & 0xFF)) / 255.0f
    };
    if (m_textcolor) CGColorRelease(m_textcolor);
    CGColorSpaceRef cspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    m_textcolor = CGColorCreate(cspace, components);
    CGColorSpaceRelease(cspace);
#elif defined (PLATFORM_ANDROID)
    JNIHelper jni;
    jni.env()->CallVoidMethod(m_jni.paint, m_jni.paint_setARGB,
                color >> 24, 0xFF & (color >> 16), 0xFF & (color >> 8), 0xFF & color);
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::set_textstyle(const TextStyle& style) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    LOGFONT lf = { 0 };
    lf.lfHeight = -style.fontsize;  // negative = pixel
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = (style.appearance & TextStyle::Bold) ? FW_HEAVY : FW_SEMIBOLD;
    lf.lfItalic = (style.appearance & TextStyle::Italic) ? TRUE : FALSE;
    lf.lfUnderline = (style.appearance & TextStyle::Underline) ? TRUE : FALSE;
    lf.lfStrikeOut = (style.appearance & TextStyle::Strike) ? TRUE : FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = NONANTIALIASED_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    wcsncpy_s(lf.lfFaceName, L"Verdana", sizeof(lf.lfFaceName) / sizeof(lf.lfFaceName[0]));
    HFONT hFont;
    hFont = CreateFontIndirect(&lf);
    if (hFont == NULL) return;
    if (m_hfont) {
        SelectObject(m_hdc, hFont);
        DeleteObject(m_hfont);
    } else {
        m_ofont = (HFONT)SelectObject(m_hdc, hFont);
    }
    m_hfont = hFont;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    if (m_font) CFRelease(m_font);
    CFMutableDictionaryRef attr = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    // family
    CFStringRef family = CFStringCreateWithCString(kCFAllocatorDefault, "Helvetica", kCFStringEncodingUTF8);    // Courier
    CFDictionaryAddValue(attr, kCTFontFamilyNameAttribute, family);
    CFRelease(family);
    // font size
    CFNumberRef fontsize = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &style.fontsize);
    CFDictionaryAddValue(attr, kCTFontSizeAttribute, fontsize);
    CFRelease(fontsize);
    // bold
    if (style.appearance & TextStyle::Bold) {
        CFStringRef bold = CFStringCreateWithCString(kCFAllocatorDefault, "Bold", kCFStringEncodingUTF8);
        CFDictionaryAddValue(attr, kCTFontStyleNameAttribute, bold);
        CFRelease(bold);
    }
    CTFontDescriptorRef descriptor = CTFontDescriptorCreateWithAttributes(attr);
    m_font = CTFontCreateWithFontDescriptor(descriptor, 0.0, NULL);
    CFRelease(attr);
    CFRelease(descriptor);
#elif defined (PLATFORM_ANDROID)
    JNIHelper jni;
    jni.env()->CallVoidMethod(m_jni.paint, m_jni.paint_setTextSize, (float)style.fontsize);
    // TODO: Bold with Typeface.DEFAULT_BOLD
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::calctext(const char* utf8, int* w, int* h) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    TCHAR      text[256];
    RECT rc = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, text, 256);
    ::DrawText(m_hdc, text, -1, &rc, DT_NOCLIP | DT_NOPREFIX | DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT);
    *w = rc.right - rc.left + 2;    // reserve +1 for shadow
    *h = rc.bottom - rc.top + 2;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    if (utf8) {
        CFAttributedStringRef str = create_string(utf8);
        CTFramesetterRef frameSetter = CTFramesetterCreateWithAttributedString(str);
        CGSize suggestedSize = CTFramesetterSuggestFrameSizeWithConstraints(
            frameSetter, /* Framesetter */
            CFRangeMake(0, 0), /* String range (entire string) */
            NULL, /* Frame attributes */
            CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX), /* Constraints (CGFLOAT_MAX indicates unconstrained) */
            NULL /* Gives the range of string that fits into the constraints, doesn't matter in your situation */
        );
        CFRelease(frameSetter);
        CFRelease(str);
        *w = suggestedSize.width + 1; // reserve +1 for shadow
        *h = suggestedSize.height + 1;
    } else {
        *w = 0;
        *h = 0;
    }
#elif defined (PLATFORM_ANDROID)
    JNIHelper jni;
    jstring s = jni.env()->NewStringUTF(utf8);
    jsize slen = jni.env()->GetStringLength(s);
    jni.env()->CallVoidMethod(m_jni.paint, m_jni.paint_getTextBounds, s, 0, slen, m_jni.rect);
    *w = jni.env()->CallIntMethod(m_jni.rect, m_jni.rect_width) + 1; // reserve +1 for shadow
    *h = jni.env()->CallIntMethod(m_jni.rect, m_jni.rect_height) + 1;
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::drawtext(const char* utf8, int x, int y, int w, int h) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    TCHAR	text[256];
    RECT rc = { x, y, x+w, y+h };
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, text, 256);
    ::DrawText(m_hdc, text, -1, &rc, DT_NOCLIP | DT_NOPREFIX | DT_LEFT | DT_TOP | DT_SINGLELINE);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    if (utf8 == NULL) return;
    // CGContextSetRGBFillColor (context, 1, 0, 1, 1);
    // CGContextFillRect (context, CGRectMake (x, y, width, height ));
    CFAttributedStringRef str = create_string(utf8);
    CTLineRef line = CTLineCreateWithAttributedString(str);
    CGContextSetTextPosition(m_context, x, m_height - h - y + 2);
    CTLineDraw(line, m_context);
    CFRelease(line);
    CFRelease(str);
#elif defined (PLATFORM_ANDROID)
    JNIHelper jni;
    jstring s = jni.env()->NewStringUTF(utf8);
    jni.env()->CallVoidMethod(m_jni.canvas, m_jni.canvas_drawText, s, (float)x, (float)(m_height - y - 2), m_jni.paint); 
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::line(unsigned int color, int x1, int y1, int x2, int y2) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    HPEN pen = CreatePen(PS_SOLID, 1, rgba2gdicolor(color));
    HPEN o = (HPEN)SelectObject(m_hdc, pen);
    MoveToEx(m_hdc, x1, y1, NULL);
    LineTo(m_hdc, x2, y2);
    SelectObject(m_hdc, o);
    DeleteObject(pen);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    CGFloat components[] = {
        ((CGFloat)(color & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 8) & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 16) & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 24) & 0xFF)) / 255.0f
    };
    CGColorSpaceRef cspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    CGColorRef      cgcolor = CGColorCreate(cspace, components);
    CGContextSetStrokeColorWithColor(m_context, cgcolor);
    CGContextSetLineWidth(m_context, 1.0);
    CGContextMoveToPoint(m_context, x1 + 1, m_height - y1 - 1);
    CGContextAddLineToPoint(m_context, x2 + 1, m_height - y2 - 1);
    CGContextStrokePath(m_context);
    CGColorRelease(cgcolor);
    CGColorSpaceRelease(cspace); 
#elif defined (PLATFORM_ANDROID)
    JNIHelper jni;
    jni.env()->CallVoidMethod(m_jni.canvas, m_jni.canvas_drawLine, (float)x1, (float)(m_height - y1), (float)x2, (float)(m_height - y2), m_jni.paint);
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void DrawableCanvas::fill(unsigned int color, int x1, int y1, int x2, int y2) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Implement
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    CGFloat components[] = {
        ((CGFloat)(color & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 8) & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 16) & 0xFF)) / 255.0f,
        ((CGFloat)((color >> 24) & 0xFF)) / 255.0f
    };
    CGColorSpaceRef cspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    CGColorRef      cgcolor = CGColorCreate(cspace, components);
    CGRect rect = CGRectMake(x1 + 1, m_height - y2 - 1, x2 - x1 + 1, y2 - y1 + 1);
    CGContextSetFillColorWithColor(m_context, cgcolor);
    CGContextFillRect(m_context, rect);
    CGColorRelease(cgcolor);
    CGColorSpaceRelease(cspace);
#elif defined (PLATFORM_ANDROID)
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
// Platform Specific: Windows
// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
#include <emmintrin.h>
COLORREF DrawableCanvas::rgba2gdicolor(uint32_t color) {
    unsigned int a = (color >> 26) & 0x3F;     // 6 bit
    unsigned int r = (color >> 18) & 0x3F;     // 6 bit
    unsigned int g = (color >> 10) & 0x3F;     // 6 bit
    unsigned int b = (color >> 2) & 0x3F;      // 6 bit
    return RGB(((a << 2) | (b >> 4)), ((b & 0x0F) << 4) | (g >> 2), ((g & 3) << 6) | r);
}
// -----------------------------------------------------------
// convert GDI buffer to gl texture, handle color space transform.
// -----------------------------------------------------------
void DrawableCanvas::update_rgba() {
    const __m128i mask_a = _mm_set_epi32(0xFC000000, 0xFC000000, 0xFC000000, 0xFC000000);
    const __m128i mask_r = _mm_set_epi32(0x00FC0000, 0x00FC0000, 0x00FC0000, 0x00FC0000);
    const __m128i mask_g = _mm_set_epi32(0x0000FC00, 0x0000FC00, 0x0000FC00, 0x0000FC00);
    const __m128i mask_b = _mm_set_epi32(0x000000FC, 0x000000FC, 0x000000FC, 0x000000FC);
    const __m128i* src = (const __m128i*) m_bmpixel;
    __m128i* des = (__m128i*) m_pixel;
    for (int j = 0; j<m_width * m_height / 4; j++) {
        __m128i s = _mm_load_si128(src);
        __m128i a = _mm_slli_epi32(s, 8);
        __m128i r = _mm_slli_epi32(s, 6);
        __m128i g = _mm_slli_epi32(s, 4);
        __m128i b = _mm_slli_epi32(s, 2);
        a = _mm_and_si128(a, mask_a);
        r = _mm_and_si128(r, mask_r);
        g = _mm_and_si128(g, mask_g);
        b = _mm_and_si128(b, mask_b);
        a = _mm_or_si128(a, r);
        g = _mm_or_si128(g, b);
        a = _mm_or_si128(a, g);
        _mm_store_si128(des, a);
        src++; des++;
    }
}
// ----------------------------------------------------------------------------
// Platform Specific: PLATFORM_MACOSX, PLATFORM_IOS
// ----------------------------------------------------------------------------
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
CFAttributedStringRef DrawableCanvas::create_string(const char* utf8) {
    CFStringRef strref = CFStringCreateWithCString(kCFAllocatorDefault, utf8, kCFStringEncodingUTF8);
    CFStringRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
    CFTypeRef values[] = { m_font, m_textcolor };
    CFDictionaryRef attributes = CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
        (const void**)&values, sizeof(keys) / sizeof(keys[0]),
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
    CFAttributedStringRef str = CFAttributedStringCreate(kCFAllocatorDefault, strref, attributes);
    CFRelease(strref);
    CFRelease(attributes);
    return str;
}
// ----------------------------------------------------------------------------
#endif
