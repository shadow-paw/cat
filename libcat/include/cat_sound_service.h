#ifndef __CAT_SOUND_SERVICE_H__
#define __CAT_SOUND_SERVICE_H__

#include <string>
#include <utility>
#include <unordered_map>
#include "cat_platform.h"
#include "cat_sound_type.h"
#include "cat_sound_effects.h"

namespace cat {
class Kernel;
// ----------------------------------------------------------------------------
//! Sound Service
class SoundService {
friend class cat::Kernel;
public:
    SoundService();
    ~SoundService();

    const SoundEffect* load_effect(const std::string& name);
    bool unload(const SoundEffect* sound);
    void play(const SoundEffect* sound, float left_volume, float right_volume);

private:
    //! Initialize service
    bool init(const PlatformSpecificData* psd);
    //! Cleanup service
    void fini();
    //! Called from kernel when the app is put to background
    void pause();
    //! Called from kernel when the app is resume to foreground
    void resume();
private:
    SoundEffects m_effects;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_SOUND_SERVICE_H__
