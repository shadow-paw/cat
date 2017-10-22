#ifndef __CAT_TYPE_H__
#define __CAT_TYPE_H__

#include "cat_platform.h"
#include "cat_time_type.h"

namespace cat {
// ----------------------------------------------------------------------------
// UI Event
// ----------------------------------------------------------------------------
struct TouchEvent {
    enum EventType { TouchDown, TouchUp, TouchMove, Scroll };
    EventType    type;
    Timestamp    timestamp;
    int          pointer_id;
    int          x, y;
    int          scroll;
    unsigned int button;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_TYPE_H__
