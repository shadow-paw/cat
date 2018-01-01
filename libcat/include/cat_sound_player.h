#ifndef __CAT_SOUND_PLAYER_H__
#define __CAT_SOUND_PLAYER_H__

#include <string>
#if defined(PLATFORM_ANDROID)
#include <SLES/OpenSLES.h>
#endif
#include "cat_platform.h"
#include "cat_data_event.h"

namespace cat {
// ----------------------------------------------------------------------------
class SoundService;
class AudioEngine;
class AudioPlayer {
friend class AudioEngine;
public:
    enum class Status { Failed, Loaded, Playing, Paused };
    EventHandler<AudioPlayer,Status> ev_status;

    AudioPlayer();
    ~AudioPlayer();
    bool play();
    bool pause();
    bool stop();

    bool is_playing();
    unsigned long duration();
private:
    bool load(AudioEngine* engine, const std::string& name);
    void unload();
private:
    bool m_loaded;
#if defined(PLATFORM_ANDROID)
    SLObjectItf m_player;
    SLPlayItf   m_play_iface;
    SLSeekItf   m_seek_iface;
    SLVolumeItf m_vol_iface;
    static void cb_prefetch(SLPrefetchStatusItf caller, void *context, SLuint32 ev);
    static void cb_playback(SLPlayItf caller, void *context, SLuint32 ev);
#endif
};
// ----------------------------------------------------------------------------
class AudioEngine {
friend class AudioPlayer;
friend class SoundService;
public:
    AudioEngine();
    ~AudioEngine();
    AudioPlayer* create_player(const std::string& uri);
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
    const PlatformSpecificData* m_psd;
#if defined(PLATFORM_ANDROID)
    SLObjectItf m_instance;
    SLEngineItf m_engine;
    SLObjectItf m_outmix;
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_SOUND_PLAYER_H__
