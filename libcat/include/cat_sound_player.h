#ifndef __CAT_SOUND_PLAYER_H__
#define __CAT_SOUND_PLAYER_H__

#include <string>
#include "cat_platform.h"
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
  #include <mfobjects.h>
  #include <mfidl.h>
  #pragma comment(lib, "shlwapi")
  #pragma comment(lib, "mf.lib")
  #pragma comment(lib, "mfplat.lib")
  #pragma comment(lib, "mfuuid.lib")
#elif defined(PLATFORM_ANDROID)
  #include <SLES/OpenSLES.h>
#endif
#include "cat_data_event.h"
#include "cat_data_copyable.h"

namespace cat {
// ----------------------------------------------------------------------------
class SoundService;
class AudioEngine;
class AudioPlayer : private NonCopyable<AudioPlayer>
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    , public IMFAsyncCallback
#endif
{
friend class AudioEngine;
public:
    enum class Status { Failed, Loaded, Playing, Paused };
    EventHandler<AudioPlayer,Status> ev_status;
    void release();

    bool play();
    bool pause();
    bool stop();

    bool is_playing();
    bool is_loaded() const { return m_loaded; }
    unsigned long duration();
private:
    bool load(AudioEngine* engine, const std::string& name);
    void unload();
private:
    AudioPlayer();
    ~AudioPlayer();

    bool m_loaded;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // IUnknown methods
    long m_ref;
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    // IMFAsyncCallback methods
    STDMETHODIMP  GetParameters(DWORD*, DWORD*);
    STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);
    // data
    IMFMediaSession* m_session;
    IMFMediaSource* m_source;
    Status m_status;
    unsigned long m_duration;
    bool m_closed;

#elif defined(PLATFORM_ANDROID)
    SLObjectItf m_player;
    SLPlayItf   m_play_iface;
    SLSeekItf   m_seek_iface;
    SLVolumeItf m_vol_iface;
    static void cb_prefetch(SLPrefetchStatusItf caller, void *context, SLuint32 ev);
    static void cb_playback(SLPlayItf caller, void *context, SLuint32 ev);
#elif defined(PLATFORM_MAC)
    void* m_player;
    void* m_delegate;
#elif defined(PLATFORM_IOS)
    void* m_player;
    void* m_delegate;
#endif
};
// ----------------------------------------------------------------------------
class AudioEngine : private NonCopyable<AudioEngine> {
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
