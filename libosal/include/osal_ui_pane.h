#ifndef __OSAL_UI_PANE_H__
#define __OSAL_UI_PANE_H__

#include "osal_ui_widget.h"
#include "osal_gfx_type.h"
#include "osal_gfx_draw2d.h"

namespace osal { namespace ui {
// ----------------------------------------------------------------------------
class Pane : public Widget {
public:
    static const int TexBackground = 0;

    Pane(osal::KernelApi* kernel, const osal::gfx::Rect2i& rect, unsigned int id = 0);
    virtual ~Pane();

    void set_draggable(bool b) { m_draggable = b; }
    bool is_draggable() const { return m_draggable; }
    void set_bounded(bool b) { m_bounded = b; }
    bool is_bounded() const { return m_bounded; }

    void set_bgeffect(osal::gfx::Draw2D::Effect effect);

protected:
    virtual bool cb_timer(osal::time::Timestamp now, int code);
    virtual bool cb_touch(const TouchEvent& ev, bool handled);
    virtual void cb_render(osal::gfx::Renderer* r, osal::time::Timestamp now);

protected:
    bool m_draggable, m_dragging;
    int  m_dragx, m_dragy;
    bool m_bounded;
    osal::gfx::Draw2D::Effect m_effect;
    osal::gfx::Texture m_effect_tex;
};
// ----------------------------------------------------------------------------
}} // namespace osal::ui

#endif // __OSAL_UI_PANE_H__
