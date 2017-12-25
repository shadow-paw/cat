#ifndef __CAT_GFX_TYPE_H__
#define __CAT_GFX_TYPE_H__

#include <stdint.h>
#include "cat_gfx_opengl.h"

namespace cat {
// ----------------------------------------------------------------------------
// Shader Data
// ----------------------------------------------------------------------------
#pragma pack(push,1)
struct Vertex2f {
    GLfloat x, y;
    GLfloat u, v;
    GLuint  color;
};
struct Vertex3f {
    GLfloat x, y, z;
    GLfloat u, v;
    GLuint  color;
};
#pragma pack(pop)
// ----------------------------------------------------------------------------
// Geometry
// ----------------------------------------------------------------------------
struct Point2i {
    int x, y;
    Point2i() = default;
    Point2i(int _x, int _y) : x(_x), y(_y) {}
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
struct TextStyle {
    enum Appearance {
        Normal = 0,
        Bold = 1 << 0,
        Italic = 1 << 1,
        Underline = 1 << 2,
        Strike = 1 << 3,
        Shadow = 1 << 4
    };
    enum Gravity {
        Left = 0,
        Top = 0,
        Right = 1 << 0,
        Bottom = 1 << 1,
        CenterHorizontal = 1 << 2,
        CenterVertical = 1 << 3,
        Center = CenterHorizontal| CenterVertical
    };
    int           appearance;
    int           gravity;
    int           fontsize;
    uint32_t      color;
    unsigned int  padding_x, padding_y;

    TextStyle() {
        appearance = TextStyle::Appearance::Normal;
        gravity = TextStyle::Gravity::Left | TextStyle::Gravity::Top;
        fontsize = 12;
        color = 0xffffffff;
        padding_x = padding_y = 0;
    }
};
// ----------------------------------------------------------------------------
} // namespace cat::gfx

#endif // __CAT_GFX_TYPE_H__
