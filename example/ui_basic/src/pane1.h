#ifndef __APP_PANE1_H__
#define __APP_PANE1_H__

#include "libcat.h"

namespace app {
// ----------------------------------------------------------------------------
class Pane1 : public cat::Pane {
public:
    cat::EventHandler<Widget,bool> ev_check;

    Pane1(cat::KernelApi* kernel_api, const cat::Rect2i& rect, unsigned int id = 0);
    virtual ~Pane1();
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_PANE1_H__
