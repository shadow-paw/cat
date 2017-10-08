#ifndef __OSAL_TYPE_H__
#define __OSAL_TYPE_H__

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
}} // namespace osal::gfx

#endif // __OSAL_TYPE_H__
