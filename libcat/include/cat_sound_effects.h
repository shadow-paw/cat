#ifndef __CAT_SOUND_EFFECTS_H__
#define __CAT_SOUND_EFFECTS_H__

#include <string>
#include <utility>
#include <unordered_map>
#include "cat_platform.h"
#include "cat_data_copyable.h"
#include "cat_sound_type.h"

namespace cat {
// ----------------------------------------------------------------------------
//! Sound Effects
class SoundEffects : private NonCopyable<SoundEffects> {
friend class SoundService;
public:
    const SoundEffect* load(const std::string& name);
    bool unload(const SoundEffect* sound);
    void play(const SoundEffect* sound, float left_volume, float right_volume);

private:
    SoundEffects();
    ~SoundEffects();

    //! Initialize service
    bool init(const PlatformSpecificData* psd);
    //! Cleanup service
    void fini();
    //! Called from kernel when the app is put to background
    void pause();
    //! Called from kernel when the app is resume to foreground
    void resume();
private:
    static const int MAX_STREAM = 32;
    const PlatformSpecificData* m_psd;
    std::unordered_map<std::string, std::pair<SoundEffect*, int>> m_sounds;
#if defined(PLATFORM_ANDROID)
    jobject m_soundpool;
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_SOUND_EFFECTS_H__
