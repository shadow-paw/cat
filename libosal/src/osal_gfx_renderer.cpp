#include "osal_gfx_renderer.h"

// NOTE: Fix for mac
#if defined(PLATFORM_MAC)
  #define glClearDepthf(x) glClearDepth(x)
#endif

using namespace osal;

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
    if (m_contextready) {
        draw2d.resize(m_width, m_height);
    }
}
// ----------------------------------------------------------------------------
void Renderer::context_lost() {
    if (!m_contextready) return;
    m_contextready = false;
    draw2d.fini();
}
// ----------------------------------------------------------------------------
bool Renderer::context_restored() {
    if (m_contextready) return true;
    if (!initGL()) return false;
    if (!draw2d.init()) return false;
    m_contextready = true;
    resize(m_width, m_height);
    return true;
}
// ----------------------------------------------------------------------------
void Renderer::begin_render() {
    draw2d.render_text();
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
