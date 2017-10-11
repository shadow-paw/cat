#ifndef __OSAL_GFX_TEXREF_H__
#define __OSAL_GFX_TEXREF_H__

#include "osal_gfx_tex.h"

namespace osal {
// ----------------------------------------------------------------------------
class TextureRef {
public:
    const Texture* tex;
    int            u1, v1, u2, v2;
    int            border_u, border_v;
    TextureRef() { 
        tex = nullptr;
        u1 = v1 = u2 = v2 = 0;
        border_u = border_v = 0;
    }
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_GFX_TEXREF_H__
