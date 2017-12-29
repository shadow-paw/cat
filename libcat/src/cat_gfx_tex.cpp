#include "cat_gfx_tex.h"
#include <stdlib.h>

using namespace cat;

// ----------------------------------------------------------------------------
Texture::Texture() {
    m_tex = 0;
    m_width = m_height = 0;
}
// ----------------------------------------------------------------------------
Texture::~Texture() {
    release();
}
// ----------------------------------------------------------------------------
void Texture::release() {
    m_width = m_height = 0;
    if (m_tex != 0) {
        glDeleteTextures(1, &m_tex);
        m_tex = 0;
    }
}
// ----------------------------------------------------------------------------
bool Texture::update(Texture::Format format, int width, int height, const void* pixel, bool antialias) {
    if (width<=0 || height<=0 ) return false;
    GLenum internal_format, src_format;
    switch (format) {
        case RGB:
            internal_format = GL_RGB;
            src_format = GL_RGB;
            break;
        case RGBA:
            internal_format = GL_RGBA;
            src_format = GL_RGBA;
            break;
        default:
            return false;
    }
    if (m_tex == 0) glGenTextures(1, &m_tex);
    bind(0);
    if ((m_width!=width || m_height!=height) || !pixel) {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, src_format, GL_UNSIGNED_BYTE, pixel);
        m_width = width;
        m_height = height;
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, src_format, GL_UNSIGNED_BYTE, pixel);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, antialias ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, antialias ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    unbind(0);
    return true;
}
// ----------------------------------------------------------------------------
bool Texture::capture_screen(const Rect2i& rect) {
    m_width = rect.size.width;
    m_height = rect.size.height;
    if (m_tex == 0) glGenTextures(1, &m_tex);
    bind(0);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    unbind(0);
    return true;
}
// ----------------------------------------------------------------------------
