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
// Geometry
// ----------------------------------------------------------------------------
struct Point2i {
    int x, y;
};
struct Size2i {
    int width, height;
};
class Rect2i {
public:
    Point2i origin;
    Size2i  size;
    Rect2i() = default;
    Rect2i(int x, int y, int w, int h) { origin.x = x; origin.y = y; size.width = w; size.height = h; }
    void set(int x, int y, int w, int h) { origin.x = x; origin.y = y; size.width = w; size.height = h; }
    bool contain(int x, int y) const { return (x >= origin.x && x <= origin.x + size.width && y >= origin.y && y <= origin.y + size.height); }
    bool contain(const Point2i& pt) const { return (pt.x >= origin.x && pt.x <= origin.x + size.width && pt.y >= origin.y && pt.y <= origin.y + size.height); }
    Rect2i operator *(float scale) {
        Rect2i r;
        r.origin.x = (int)(origin.x*scale);
        r.origin.y = (int)(origin.y*scale);
        r.size.width = (int)(size.width*scale);
        r.size.height = (int)(size.height*scale);
        return r;
    }
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_TYPE_H__
