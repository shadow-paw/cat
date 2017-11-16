#ifndef __APP_TESTPANE_H__
#define __APP_TESTPANE_H__

#include "libcat.h"

namespace app {
// ----------------------------------------------------------------------------
class TestPane : public cat::Pane {
public:
    TestPane(cat::KernelApi* kernel, const cat::Rect2i& rect, unsigned int id = 0);
    virtual ~TestPane();

protected:
    virtual void cb_render(cat::Renderer* r, cat::Timestamp now);
private:
    const cat::Shader* m_shader;
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_TESTPANE_H__
