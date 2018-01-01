#ifndef __CAT_SOUND_TYPE_H__
#define __CAT_SOUND_TYPE_H__

#include <string>
#include "cat_platform.h"

namespace cat {
// ----------------------------------------------------------------------------
struct SoundEffect {
    std::string name;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO
#elif defined(PLATFORM_ANDROID)
    jint sound_id;
#else
    #error Not Implemented!
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_SOUND_TYPE_H__
