#include "osal_gfx_renderer.h"

// NOTE: Fix for mac
#if defined(PLATFORM_MACOSX)
  #define glClearDepthf(x) glClearDepth(x)
#endif

using namespace osal::gfx;

// ----------------------------------------------------------------------------
Renderer::Renderer() {
    m_contextready = false;
    m_dirty = false;
    m_width = m_height = 0;
}
// ----------------------------------------------------------------------------
Renderer::~Renderer() {
    fini();
}
// ----------------------------------------------------------------------------
bool Renderer::init() {
    return true;
}
// ----------------------------------------------------------------------------
void Renderer::fini() {
}
// ----------------------------------------------------------------------------
void Renderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_dirty = true;
}
// ----------------------------------------------------------------------------
void Renderer::context_lost() {
    if (!m_contextready) return;
    m_contextready = false;
}
// ----------------------------------------------------------------------------
bool Renderer::context_restored() {
    if (m_contextready) return true;
    if (!initGL()) return false;
    m_contextready = true;
    resize(m_width, m_height);
    return true;
}
// ----------------------------------------------------------------------------
void Renderer::begin_render() {
    m_dirty = false;
    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // | GL_STENCIL_BUFFER_BIT);
}
// ----------------------------------------------------------------------------
void Renderer::end_render() {
}
// ----------------------------------------------------------------------------
bool Renderer::initGL() {
    glDepthMask(true);
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}
// ----------------------------------------------------------------------------
