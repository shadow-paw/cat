#ifndef __OSAL_UI_LABEL_H__
#define __OSAL_UI_LABEL_H__

#include "osal_ui_widget.h"
#include "osal_gfx_type.h"

namespace osal { namespace ui {
// ----------------------------------------------------------------------------
class Label : public Widget {
public:
    static const int TexBackground = 0;

    Label(osal::KernelApi* kernel, const osal::gfx::Rect2i& rect, unsigned int id = 0);
    virtual ~Label();

    void             set_text(const std::string& s) { m_text = s; }
    std::string      get_text() const { return m_text; }
    void             set_textstyle(const osal::gfx::TextStyle& style) { m_textstyle = style; }
    const osal::gfx::TextStyle& get_textstyle() const { return m_textstyle; }
    void             set_textsize(int size) { m_textstyle.fontsize = size; }

protected:
    virtual void cb_render(osal::gfx::Renderer* r, osal::time::Timestamp now);

protected:
    std::string  m_text;
    osal::gfx::TextStyle m_textstyle;
};
// ----------------------------------------------------------------------------
}} // namespace osal::ui

#endif // __OSAL_UI_LABEL_H__
