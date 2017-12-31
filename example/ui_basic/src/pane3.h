#ifndef __APP_PANE3_H__
#define __APP_PANE3_H__

#include "libcat.h"

namespace app {
// ----------------------------------------------------------------------------
class Pane3 : public cat::Pane {
public:
    Pane3(cat::KernelApi* kernel_api, const cat::Rect2i& rect, unsigned int id = 0);
    virtual ~Pane3();
protected:
    virtual void cb_context_lost();
    virtual bool cb_context_restored();
    virtual void cb_resize();
    virtual void cb_render(cat::Renderer* r, cat::Timestamp now);
private:
    const cat::Shader* m_shader;
    int m_detail_level;
    cat::Size2i m_texsize;
    cat::Texture m_tex[3];
    void resize_tex();
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_PANE3_H__
