#ifndef __CAT_UI_PANE_H__
#define __CAT_UI_PANE_H__

#include "cat_ui_widget.h"
#include "cat_gfx_type.h"
#include "cat_gfx_draw2d.h"

namespace cat {
// ----------------------------------------------------------------------------
class Pane : public Widget {
public:
    static const int TexBackground = 0;

    Pane(KernelApi* kernel_api, const Rect2i& rect, unsigned int id = 0);
    virtual ~Pane();

    void set_draggable(bool b) { m_draggable = b; }
    bool is_draggable() const { return m_draggable; }
    void set_bounded(bool b) { m_bounded = b; }
    bool is_bounded() const { return m_bounded; }

protected:
    virtual bool cb_timer(Timestamp now, int code);
    virtual bool cb_touch(const TouchEvent& ev, bool handled);
    virtual void cb_render(Renderer* r, Timestamp now);

protected:
    bool m_draggable, m_dragging;
    int  m_dragx, m_dragy;
    bool m_bounded;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_PANE_H__
