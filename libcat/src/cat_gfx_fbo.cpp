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
    // save states
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_state.fbo);
    glGetIntegerv(GL_VIEWPORT, m_state.viewport);
    glViewport(0, 0, tex.width(), tex.height());
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.m_tex, 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
}
// ----------------------------------------------------------------------------
void FBO::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_state.fbo);
    glViewport(m_state.viewport[0], m_state.viewport[1], m_state.viewport[2], m_state.viewport[3]);
}
// ----------------------------------------------------------------------------

