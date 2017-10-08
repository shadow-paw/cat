#ifndef __OSAL_EVENT_H__
#define __OSAL_EVENT_H__

#include "osal_type.h"

namespace osal {
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
} // namespace Graphite

#endif // __OSAL_EVENT_H__
