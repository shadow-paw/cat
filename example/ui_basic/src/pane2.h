#ifndef __APP_PANE2_H__
#define __APP_PANE2_H__

#include "libcat.h"

namespace app {
// ----------------------------------------------------------------------------
class Pane2 : public cat::Pane {
public:
    Pane2(cat::KernelApi* kernel_api, const cat::Rect2i& rect, unsigned int id = 0);
    virtual ~Pane2();
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_PANE2_H__
