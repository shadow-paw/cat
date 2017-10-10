#ifndef __APP_TESTPANE_H__
#define __APP_TESTPANE_H__

#include "libosal.h"

namespace app {
// ----------------------------------------------------------------------------
class TestPane : public osal::ui::Pane {
public:
    TestPane(osal::KernelApi* kernel, const osal::gfx::Rect2i& rect, unsigned int id = 0);
    virtual ~TestPane();

protected:
    virtual void cb_render(osal::gfx::Renderer* r, osal::time::Timestamp now);
private:
    const osal::gfx::Shader* m_shader;
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_TESTPANE_H__
