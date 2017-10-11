#include "osal_ui_label.h"
#include "osal_gfx_renderer.h"

using namespace osal;

// ----------------------------------------------------------------------------
Label::Label(KernelApi* kernel, const Rect2i& rect, unsigned int id) : Widget(kernel, rect, id) {
    m_texrefs.resize(1);
    // m_textstyle.gravity = TextStyle::Gravity::CenterHorizontal | TextStyle::Gravity::CenterVertical;
}
// ----------------------------------------------------------------------------
Label::~Label() {
}
// ----------------------------------------------------------------------------
void Label::cb_render(Renderer* r, Timestamp now) {
    r->draw2d.fill(m_absrect, m_bgcolor, m_texrefs[TexBackground], now);
    r->draw2d.drawtext(m_absrect, m_text, m_textstyle);
}
// ----------------------------------------------------------------------------
