#include <stdlib.h>
#include "osal_gfx_canvas.h"

using namespace osal;

// ----------------------------------------------------------------------------
DrawableCanvas::DrawableCanvas() {
    m_width = m_height = 0;
    m_hdc = NULL;
    m_hbitmap = m_obitmap = NULL;
    m_bmpixel = nullptr;
    m_pixel = nullptr;
}
// ----------------------------------------------------------------------------
DrawableCanvas::~DrawableCanvas() {
    release();
}
// ----------------------------------------------------------------------------
bool DrawableCanvas::resize(int width, int height) {
    // create DIB
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
}
// ----------------------------------------------------------------------------
void DrawableCanvas::release() {
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
}
// ----------------------------------------------------------------------------
void DrawableCanvas::texImage2D(Texture& tex) {
    GdiFlush();
    update_rgba();
    tex.update(Texture::Format::RGBA, m_width, m_height, m_pixel);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::clear(unsigned int color) {
    RECT rc = { 0, 0, m_width, m_height };
    HBRUSH hBrush = CreateSolidBrush(rgba2gdicolor(color));
    FillRect(m_hdc, &rc, hBrush);
    DeleteObject(hBrush);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::set_textcolor(unsigned int color) {
    SetTextColor(m_hdc, rgba2gdicolor(color));
}
// ----------------------------------------------------------------------------
void DrawableCanvas::set_textstyle(const TextStyle& style) {
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
}
// ----------------------------------------------------------------------------
void DrawableCanvas::calctext(const char* utf8, int* w, int* h) {
    TCHAR      text[256];
    RECT rc = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, text, 256);
    ::DrawText(m_hdc, text, -1, &rc, DT_NOCLIP | DT_NOPREFIX | DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT);
    *w = rc.right - rc.left + 2;    // reserve +1 for shadow
    *h = rc.bottom - rc.top + 2;
}
// ----------------------------------------------------------------------------
void DrawableCanvas::drawtext(const char* utf8, int x, int y, int w, int h) {
    TCHAR	text[256];
    RECT rc = { x, y, x+w, y+h };
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, text, 256);
    ::DrawText(m_hdc, text, -1, &rc, DT_NOCLIP | DT_NOPREFIX | DT_LEFT | DT_TOP | DT_SINGLELINE);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::line(unsigned int color, int x1, int y1, int x2, int y2) {
    HPEN pen = CreatePen(PS_SOLID, 1, rgba2gdicolor(color));
    HPEN o = (HPEN)SelectObject(m_hdc, pen);
    MoveToEx(m_hdc, x1, y1, NULL);
    LineTo(m_hdc, x2, y2);
    SelectObject(m_hdc, o);
    DeleteObject(pen);
}
// ----------------------------------------------------------------------------
void DrawableCanvas::fill(unsigned int color, int x1, int y1, int x2, int y2) {
    // TODO: Implement
}
// ----------------------------------------------------------------------------
// Platform Specific: Windows
// ----------------------------------------------------------------------------
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
