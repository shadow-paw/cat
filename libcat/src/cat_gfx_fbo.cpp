#include "cat_gfx_fbo.h"
#include "cat_util_log.h"

using namespace cat;

// ----------------------------------------------------------------------------
FBO::FBO() {
    m_fbo = 0;
}
// ----------------------------------------------------------------------------
FBO::~FBO() {
    fini();
}
// ----------------------------------------------------------------------------
void FBO::fini() {
    if (m_fbo) { glDeleteFramebuffers(1, &m_fbo); m_fbo = 0; }
}
// ----------------------------------------------------------------------------
bool FBO::init() {
    if (m_fbo == 0) glGenFramebuffers(1, &m_fbo);
    return m_fbo != 0;
}
// ----------------------------------------------------------------------------
void FBO::bind(const Texture& tex) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.m_tex, 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    glGetIntegerv(GL_VIEWPORT, m_viewport);
    glViewport(0, 0, tex.width(), tex.height());
}
// ----------------------------------------------------------------------------
void FBO::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
}
// ----------------------------------------------------------------------------
