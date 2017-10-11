#ifndef __OSAL_UI_SILDER_H__
#define __OSAL_UI_SILDER_H__

#include "osal_ui_widget.h"
#include "osal_gfx_type.h"

namespace osal {
// ----------------------------------------------------------------------------
class Slider : public Widget {
public:
    enum Orentation { Horizontal, Vertical };
    static const int TexBackground = 0;
    static const int TexThumb = 1;

    // Event Handlers
    EventHandlers<int> ev_slide;

    Slider(KernelApi* kernel, const Rect2i& rect, unsigned int id = 0);
    virtual ~Slider();

    void       set_orentation(Orentation o);
    Orentation get_orentation() const { return m_orentation; }
    void       set_min(int min);
    void       set_max(int max);
    void       set_range(int min, int max);
    void       set_pos(int pos);
    int        get_min() const { return m_min; }
    int        get_max() const { return m_max; }
    int        get_pos() const { return m_pos; }

protected:
    virtual void cb_resize();
    virtual bool cb_touch(const TouchEvent& ev, bool handled);
    virtual void cb_render(Renderer* r, Timestamp now);

protected:
    Orentation m_orentation;
    int    m_min, m_max, m_pos;
    Rect2i m_thumbrc;
    bool   m_dragging;
    int    m_dragx, m_dragy;
    void update_thumbrc();
    int choose_pos(int x, int y) const;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_UI_SILDER_H__
