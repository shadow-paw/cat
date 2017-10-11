#include <ApplicationServices/ApplicationServices.h>
#include <stdlib.h>
#include "mac/osal_gfx_canvas.h"

using namespace osal;

// ----------------------------------------------------------------------------
DrawableCanvas::DrawableCanvas() {
    m_width = m_height = 0;
    m_pixel = nullptr;
    m_context = nullptr;
    m_font = nullptr;
    m_textcolor = nullptr;
}
// ----------------------------------------------------------------------------
DrawableCanvas::~DrawableCanvas() {
    release();
}
// ----------------------------------------------------------------------------
bool DrawableCanvas::resize(int width, int height) {
    uint32_t* new_pixel = (uint32_t*)realloc(m_pixel, width * height * sizeof(uint32_t));
    if (!new_pixel) return false;
    m_width = width;
    m_height = height;
    m_pixel = new_pixel;
    // Create Context
    CGColorSpaceRef cspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    m_context = CGBitmapContextCreate(m_pixel, width, height, 8, 4*m_width, cspace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(cspace);
    // Invert transform
    CGContextSetTextMatrix(m_context, CGAffineTransformIdentity);
    CGContextTranslateCTM(m_context, 0, height);
    CGContextScaleCTM(m_context, 1.0, -1.0);
    // Clear
    set_textcolor(0xffffffff);
    clear(0);
    return true;
}
// ----------------------------------------------------------------------------
void DrawableCanvas::release() {
    m_width = m_height = 0;
    if (m_font) {
        CFRelease(m_font); m_font = nullptr;
    }
    if (m_textcolor) {
        CFRelease(m_textcolor); m_textcolor = nullptr;
    }
    if (m_context) {
        CGContextRelease(m_context); m_context = nullptr;
    }
    if (m_pixel) {
        free(m_pixel); m_pixel = nullptr;
    }
}
// ----------------------------------------------------------------------------
void DrawableCanvas::texImage2D(Texture& tex) {
    CGContextFlush(m_context);
    tex.bind(0);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixel);
    tex.unbind(0);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::clear(unsigned int color) {
    if (color & 0xff000000) {
        CGFloat components[] = {
            ((CGFloat)( color      & 0xFF))/255.0f,
            ((CGFloat)((color>> 8) & 0xFF))/255.0f,
            ((CGFloat)((color>>16) & 0xFF))/255.0f,
            ((CGFloat)((color>>24) & 0xFF))/255.0f
        };
        CGContextSetFillColor(m_context, components);
        CGContextFillRect(m_context, CGRectMake(0, 0, m_width, m_height));
    } else {
        CGContextClearRect(m_context, CGRectMake(0, 0, m_width, m_height));
    }
}
// ----------------------------------------------------------------------------
void DrawableCanvas::set_textcolor(unsigned int color) {
    CGFloat components[] = {
        ((CGFloat)( color      & 0xFF))/255.0f,
        ((CGFloat)((color>> 8) & 0xFF))/255.0f,
        ((CGFloat)((color>>16) & 0xFF))/255.0f,
        ((CGFloat)((color>>24) & 0xFF))/255.0f
    };
    if (m_textcolor) CGColorRelease (m_textcolor);
    CGColorSpaceRef cspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    m_textcolor = CGColorCreate(cspace, components);
    CGColorSpaceRelease(cspace);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::set_textstyle(const TextStyle& style) {
    if (m_font ) CFRelease(m_font);
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
}
// ----------------------------------------------------------------------------
void DrawableCanvas::calctext(const char* utf8, int* w, int* h) {
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
        *w = suggestedSize.width +1; // reserve +1 for shadow
        *h = suggestedSize.height +1;
    } else {
        *w = 0;
        *h = 0;
    }
}
// ----------------------------------------------------------------------------
void DrawableCanvas::drawtext(const char* utf8, int x, int y, int w, int h) {
    if ( utf8 == NULL ) return;
    // CGContextSetRGBFillColor (context, 1, 0, 1, 1);
    // CGContextFillRect (context, CGRectMake (x, y, width, height ));
    CFAttributedStringRef str = create_string(utf8);
    CTLineRef line = CTLineCreateWithAttributedString(str);
    CGContextSetTextPosition(m_context, x, m_height - h - y +2);
    CTLineDraw(line, m_context);
    CFRelease(line);
    CFRelease(str);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::line(unsigned int color, int x1, int y1, int x2, int y2) {
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
}
// ----------------------------------------------------------------------------
void DrawableCanvas::fill(unsigned int color, int x1, int y1, int x2, int y2) {
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
}
// ----------------------------------------------------------------------------
// Platform Specific: PLATFORM_MACOSX, PLATFORM_IOS
// ----------------------------------------------------------------------------
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
