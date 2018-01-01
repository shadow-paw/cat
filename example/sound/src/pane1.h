#ifndef __APP_PANE1_H__
#define __APP_PANE1_H__

#include "libcat.h"

namespace app {
// ----------------------------------------------------------------------------
class Pane1 : public cat::Pane {
public:
    Pane1(cat::KernelApi* kernel_api, const cat::Rect2i& rect, unsigned int id = 0);
    virtual ~Pane1();

protected:
    virtual void cb_pause(bool paused);

private:
    const cat::SoundEffect* m_sfx;
    cat::AudioPlayer* m_audio;
};
// ----------------------------------------------------------------------------
} // namespace app

#endif // __APP_PANE1_H__
