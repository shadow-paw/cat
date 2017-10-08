#ifndef __OSAL_GFX_TYPE_H__
#define __OSAL_GFX_TYPE_H__

#include "osal_platform.h"

namespace osal { namespace gfx {
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
        CenterVertical = 1 << 3
    };
    int           appearance;
    int           gravity;
    int           fontsize;
    uint32_t      color;
    unsigned int  padding_x, padding_y;

    TextStyle() {
        appearance = TextStyle::Appearance::Normal;
        gravity = TextStyle::Gravity::Left | TextStyle::Gravity::Top;
        fontsize = 14;
        color = 0xffffffff;
        padding_x = padding_y = 0;
    }
};
// ----------------------------------------------------------------------------
}} // namespace osal::gfx

#endif // __OSAL_GFX_TYPE_H__
