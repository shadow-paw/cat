#ifndef __CAT_UI_EFFECTVIEW_H__
#define __CAT_UI_EFFECTVIEW_H__

#include "cat_ui_widget.h"
#include "cat_gfx_type.h"

namespace cat {
// ----------------------------------------------------------------------------
class EffectView : public Widget {
public:
    static const int TexBackground = 0;

    EffectView(KernelApi* kernel_api, const Rect2i& rect, unsigned int id = 0);
    virtual ~EffectView();
    void set_effect(Draw2D::Effect effect, int detail_level = 1);
protected:
    virtual void cb_context_lost();
    virtual bool cb_context_restored();
    virtual void cb_resize();
    virtual void cb_render(Renderer* r, Timestamp now);

protected:
    Draw2D::Effect m_effect;
    int m_detail_level;
    Size2i m_texsize;
    cat::Texture m_tex[3];
    void resize_tex();
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_EFFECTVIEW_H__
