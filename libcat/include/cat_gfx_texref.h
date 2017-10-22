#ifndef __CAT_GFX_TEXREF_H__
#define __CAT_GFX_TEXREF_H__

#include "cat_gfx_tex.h"

namespace cat {
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
} // namespace cat

#endif // __CAT_GFX_TEXREF_H__
