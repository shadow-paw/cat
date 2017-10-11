#include <stdlib.h>
#include "osal_gfx_drawable.h"

using namespace osal;

// ----------------------------------------------------------------------------
Drawable::Drawable() {
    m_width = m_height = 0;
    m_dirty = false;
}
// ----------------------------------------------------------------------------
Drawable::~Drawable() {
    release();
}
// ----------------------------------------------------------------------------
bool Drawable::resize(int width, int height) {
    if (m_width==width && m_height==height) return true;
    if ((width % 8)!=0) return false;
    release();
    m_dirty = true;
    m_width = width;
    m_height = height;
    m_tex.update(Texture::Format::RGBA, width, height, nullptr);
    m_canvas.resize(width, height);
    m_dirty = true;
    return true;
}
// ----------------------------------------------------------------------------
void Drawable::release() {
    m_tex.release();
    m_width = m_height = 0;
    m_textstyle.fontsize = 0;
    m_canvas.release();
}
// ----------------------------------------------------------------------------
void Drawable::begin_draw() {
}
// ----------------------------------------------------------------------------
void Drawable::end_draw() {
    m_dirty = true;
}
// ----------------------------------------------------------------------------
bool Drawable::update() {
    if (!m_dirty) return false;
    m_dirty = false;
    m_canvas.texImage2D(m_tex);
    return true;
}
// ----------------------------------------------------------------------------
void Drawable::clear(uint32_t color) {
    m_canvas.clear(color);
}
// ----------------------------------------------------------------------------
void Drawable::set_textstyle(const TextStyle& style) {
    if (m_textstyle.appearance==style.appearance && m_textstyle.fontsize==style.fontsize && m_textstyle.color==style.color) return;
    m_canvas.set_textstyle(style);
    m_textstyle = style;
}
// ----------------------------------------------------------------------------
void Drawable::calctext(Size2i* size, const std::string& utf8, const TextStyle& style) {
    calctext(size, utf8.c_str(), style);
}
// ----------------------------------------------------------------------------
void Drawable::calctext(Size2i* size, const char* utf8, const TextStyle& style) {
    if (!utf8 || !size) return;
    set_textstyle(style);
    m_canvas.calctext(utf8, &size->width, &size->height);
}
// ----------------------------------------------------------------------------
void Drawable::drawtext(const Rect2i& rect, const std::string& utf8, const TextStyle& style) {
    drawtext(rect, utf8.c_str(), style);
}
// ----------------------------------------------------------------------------
void Drawable::drawtext(const Rect2i& rect, const char* utf8, const TextStyle& style) {
    if (!utf8) return;
    set_textstyle(style);
    if (style.appearance & TextStyle::Shadow) {
        uint32_t shadow = (style.color & 0x00FCFCFC) >> 2;
        if ((shadow & 0xF0F0F0)) {
            m_canvas.set_textcolor(0xc0000000 | shadow);
            m_canvas.drawtext(utf8, rect.origin.x + 1, rect.origin.y + 1, rect.size.width - 1, rect.size.height - 1);
        }
    }
    m_canvas.set_textcolor(style.color);
    m_canvas.drawtext(utf8, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}
// ----------------------------------------------------------------------------
