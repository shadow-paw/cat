#ifndef __OSAL_TYPE_H__
#define __OSAL_TYPE_H__

#include "osal_platform.h"

namespace osal {
// ----------------------------------------------------------------------------
typedef unsigned long Timestamp;

// ----------------------------------------------------------------------------
// UI Event
// ----------------------------------------------------------------------------
struct TouchEvent {
    enum EventType { TouchDown, TouchUp, TouchMove, Scroll };
    EventType     type;
    Timestamp     timestamp;
    int           pointer_id;
    int           x, y;
    int           scroll;
    unsigned int  button;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_TYPE_H__
