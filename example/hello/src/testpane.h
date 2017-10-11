#ifndef __APP_TESTPANE_H__
#define __APP_TESTPANE_H__

#include "libosal.h"

namespace app {
// ----------------------------------------------------------------------------
class TestPane : public osal::Pane {
public:
    TestPane(osal::KernelApi* kernel, const osal::Rect2i& rect, unsigned int id = 0);
    virtual ~TestPane();

protected:
    virtual void cb_render(osal::Renderer* r, osal::Timestamp now);
private:
    const osal::Shader* m_shader;
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_TESTPANE_H__
