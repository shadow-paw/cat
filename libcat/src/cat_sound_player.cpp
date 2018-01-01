#include "cat_sound_player.h"
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
  #include <shlwapi.h>
  #include <mfapi.h>
#elif defined(PLATFORM_ANDROID)
  #include <SLES/OpenSLES_Android.h>
  #include <android/asset_manager_jni.h>
#elif defined(PLATFORM_MAC)
  #import <AppKit/AppKit.h>
#elif defined(PLATFORM_IOS)
  #import <AVFoundation/AVFoundation.h>
#endif
#include "cat_util_string.h"
#include "cat_util_log.h"

using namespace cat;

// ----------------------------------------------------------------------------
// NOTE: Due to circular refernece in Windows meda foundation design.
//       AudioPlayer have resource leak.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Platform Specific: Windows
// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
HRESULT AudioPlayer::QueryInterface(REFIID riid, void** ppv) {
    static const QITAB qit[] = {
        QITABENT(AudioPlayer, IMFAsyncCallback),
        {0}
    };
    return QISearch(this, qit, riid, ppv);
}
// ----------------------------------------------------------------------------
ULONG AudioPlayer::AddRef() {
    return InterlockedIncrement(&m_ref);
}
// ----------------------------------------------------------------------------
ULONG AudioPlayer::Release() {
    ULONG count = InterlockedDecrement(&m_ref);
    if (count == 0) delete this;
    return count;
}
// ----------------------------------------------------------------------------
STDMETHODIMP AudioPlayer::GetParameters(DWORD*, DWORD*) {
    return E_NOTIMPL;
}
// ----------------------------------------------------------------------------
HRESULT AudioPlayer::Invoke(IMFAsyncResult *pResult) {
    MediaEventType meType = MEUnknown;  // Event type
    IMFMediaEvent *pEvent = NULL;
    if (!m_session) return S_OK;
    // Get the event from the event queue.
    if (FAILED(m_session->EndGetEvent(pResult, &pEvent))) goto done;
    if (FAILED(pEvent->GetType(&meType))) goto done;
    if (meType == MESessionClosed) {
        // The session was closed. 
        this->Release();
    } else {
        // For all other events, get the next event in the queue.
        if (FAILED(m_session->BeginGetEvent(this, NULL))) goto done;
    }
    // Check the application state. 
    // If a call to IMFMediaSession::Close is pending, it means the 
    // application is waiting on the m_hCloseEvent event and
    // the application's message loop is blocked. 
    // Otherwise, post a private window message to the application. 
    // if (m_state != Closing) {
        // Leave a reference count on the event.
    //    pEvent->AddRef();
        // PostMessage(m_hwndEvent, WM_APP_PLAYER_EVENT, (WPARAM)pEvent, (LPARAM)meType);
    //}
done:
    if (pEvent) pEvent->Release();
    return S_OK;
}
// ----------------------------------------------------------------------------
// Platform Specific: Android
// ----------------------------------------------------------------------------
#elif defined(PLATFORM_ANDROID)
void AudioPlayer::cb_prefetch(SLPrefetchStatusItf caller, void *context, SLuint32 ev) {
    AudioPlayer* self = static_cast<AudioPlayer*>(context);
    SLpermille level = 0;
    (*caller)->GetFillLevel(caller, &level);
    SLuint32 status;
    (*caller)->GetPrefetchStatus(caller, &status);
    // If error occurs, both event comes at once and level is zero
    if ((ev & (SL_PREFETCHEVENT_STATUSCHANGE | SL_PREFETCHEVENT_FILLLEVELCHANGE)) == (SL_PREFETCHEVENT_STATUSCHANGE|SL_PREFETCHEVENT_FILLLEVELCHANGE)
        && (level == 0) && (status == SL_PREFETCHSTATUS_UNDERFLOW)) {
        self->ev_status.call(self, Status::Failed);
    } else if (level == 1000) {
        self->m_loaded = true;
        self->ev_status.call(self, Status::Loaded);
    }
}
// ----------------------------------------------------------------------------
void AudioPlayer::cb_playback(SLPlayItf caller, void *context, SLuint32 ev) {
    AudioPlayer* self = static_cast<AudioPlayer*>(context);
    if ((ev & SL_PLAYEVENT_HEADATEND) == SL_PLAYEVENT_HEADATEND) {
        self->ev_status.call(self, Status::Paused);
    }
}
// ----------------------------------------------------------------------------
// Platform Specific: Mac
// ----------------------------------------------------------------------------
#elif defined(PLATFORM_MAC)
@interface AudioPlayerDelegate : NSObject<NSSoundDelegate>
@property (nonatomic) AudioPlayer* player;
@end
@implementation AudioPlayerDelegate
- (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)flag {
    if (self.player) self.player->ev_status.call(self.player, AudioPlayer::Status::Paused);
}
@end
// ----------------------------------------------------------------------------
// Platform Specific: ios
// ----------------------------------------------------------------------------
#elif defined(PLATFORM_IOS)
@interface AudioPlayerDelegate : NSObject<AVAudioPlayerDelegate>
@property (nonatomic) AudioPlayer* player;
@end
@implementation AudioPlayerDelegate
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag {
    if (self.player) self.player->ev_status.call(self.player, AudioPlayer::Status::Paused);
}
@end
#endif
// ----------------------------------------------------------------------------
// AudioPlayer
// ----------------------------------------------------------------------------
AudioPlayer::AudioPlayer() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    m_ref = 0;
    m_session = nullptr;
    m_source = nullptr;
#elif defined(PLATFORM_ANDROID)
    m_player = nullptr;
    m_play_iface = nullptr;
    m_seek_iface = nullptr;
    m_vol_iface = nullptr;
#elif defined(PLATFORM_MAC)
    m_player = nullptr;
    m_delegate = nullptr;
#elif defined(PLATFORM_IOS)
    m_player = nullptr;
    m_delegate = nullptr;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
AudioPlayer::~AudioPlayer() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // NOTHING
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    unload();
#elif
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void AudioPlayer::release() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    unload();
    this->Release();
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    delete this;
#elif
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool AudioPlayer::load(AudioEngine* engine, const std::string& name) {
    m_loaded = false;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    IMFSourceResolver* resolver = NULL;
    MF_OBJECT_TYPE type = MF_OBJECT_INVALID;
    IUnknown* isource = NULL;
    IMFPresentationDescriptor* pd = NULL;
    IMFTopology* topology = NULL;
    DWORD streams = 0;
    UINT64 duration = 0;

    m_closed = false;

    if (FAILED(MFCreateMediaSession(NULL, &m_session))) return false;
    if (FAILED(m_session->BeginGetEvent((IMFAsyncCallback*)this, NULL))) goto fail;
    // media source
    if (FAILED(MFCreateSourceResolver(&resolver))) goto fail;
    if (FAILED(resolver->CreateObjectFromURL(
                StringUtil::make_tstring("../" + name).c_str(),
                MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
                NULL,                       // Optional property store.
                &type,                      // Receives the created object type. 
                &isource                    // Receives a pointer to the media source.
            ))) goto fail;
    // Get the IMFMediaSource interface from the media source.
    isource->QueryInterface(IID_PPV_ARGS(&m_source));
    // Create the presentation descriptor for the media source.
    if (FAILED(m_source->CreatePresentationDescriptor(&pd))) goto fail;
    if (FAILED(MFCreateTopology(&topology))) goto fail;
    if (FAILED(pd->GetStreamDescriptorCount(&streams))) goto fail;
    if (FAILED(pd->GetUINT64(MF_PD_DURATION, (UINT64*)&duration))) goto fail;
    m_duration = (unsigned long)duration/10000;
    // For each stream, create the topology nodes and add them to the topology.
    for (DWORD i=0; i<streams; i++) {
        IMFActivate* pSinkActivate = NULL;
        IMFTopologyNode* pSourceNode = NULL;
        IMFTopologyNode* pOutputNode = NULL;
        IMFStreamDescriptor* sd = NULL;
        IMFMediaTypeHandler* handler = NULL;
        BOOL selected = FALSE;
        GUID guidMajorType;
        if (FAILED(pd->GetStreamDescriptorByIndex(i, &selected, &sd))) goto fail;
        if (!selected) continue;
        if (FAILED(sd->GetMediaTypeHandler(&handler))) {
            sd->Release();
            continue;
        }
        if (FAILED(handler->GetMajorType(&guidMajorType))) {
            handler->Release();
            sd->Release();
            continue;
        }
        handler->Release();
        sd->Release();
        if (MFMediaType_Audio == guidMajorType) {
            // Create the audio renderer.
            if (FAILED(MFCreateAudioRendererActivate(&pSinkActivate))) {
                continue;
            }
            // Add a source node to a topology.
            if (FAILED(MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pSourceNode))) {
                pSinkActivate->Release();
                continue;
            }
            if (FAILED(pSourceNode->SetUnknown(MF_TOPONODE_SOURCE, m_source)) ||
                FAILED(pSourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pd)) ||
                FAILED(pSourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, sd)))
            {
                pSourceNode->Release();
                pSinkActivate->Release();
                continue;
            }
            // output node
            if (FAILED(MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pOutputNode))) {
                pSourceNode->Release();
                pSinkActivate->Release();
                continue;
            }
            if (FAILED(pOutputNode->SetObject(pSinkActivate)) ||
                FAILED(pOutputNode->SetUINT32(MF_TOPONODE_STREAMID, 0)) ||
                FAILED(pOutputNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE)))
            {
                pOutputNode->Release();
                pSourceNode->Release();
                pSinkActivate->Release();
                continue;
            }
            // connect nodes
            if (FAILED(topology->AddNode(pSourceNode)) || FAILED(topology->AddNode(pOutputNode)) ||
                FAILED(pSourceNode->ConnectOutput(0, pOutputNode, 0)))
            {
                pOutputNode->Release();
                pSourceNode->Release();
                pSinkActivate->Release();
                continue;
            }
        }
    }
    if (FAILED(m_session->SetTopology(0, topology))) goto fail;
    if (topology) topology->Release();
    if (pd) pd->Release();
    if (isource) isource->Release();
    if (resolver) resolver->Release();
    this->AddRef();
    m_status = Status::Loaded;
    ev_status.call(this, m_status);
    return true;
fail:
    if (topology) topology->Release();
    if (pd) pd->Release();
    if (m_source) { m_source->Release(); m_source = nullptr; }
    if (isource) isource->Release();
    if (resolver) resolver->Release();
    unload();
    return false;
#elif defined(PLATFORM_ANDROID)
    SLPrefetchStatusItf prefetch_iface;
    // input
    //SLDataLocator_URI loc_uri = {SL_DATALOCATOR_URI, (SLchar*)uri.c_str()};
    //SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    //SLDataSource audioSrc = {&loc_uri, &format_mime};
    JNIHelper jni;
    if (name[0] != '/') return false;
    AAssetManager* mgr = AAssetManager_fromJava(jni.env(), engine->m_psd->asset_manager);
    AAsset* asset = AAssetManager_open(mgr, name.substr(1).c_str(), AASSET_MODE_UNKNOWN);
    if (!asset) return false;
    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    AAsset_close(asset);
    if (fd < 0) return false;
    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};
    // output
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, engine->m_outmix};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_PREFETCHSTATUS, SL_IID_SEEK, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    if ((*engine->m_engine)->CreateAudioPlayer(engine->m_engine, &m_player, &audioSrc, &audioSnk, 3, ids, req) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_player)->Realize(m_player, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) goto fail;
    // query interfaces
    if ((*m_player)->GetInterface(m_player, SL_IID_PLAY, &m_play_iface) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_player)->GetInterface(m_player, SL_IID_SEEK, &m_seek_iface) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_player)->GetInterface(m_player, SL_IID_VOLUME, &m_vol_iface) != SL_RESULT_SUCCESS) goto fail;
    // prefetch event
    if ((*m_player)->GetInterface(m_player, SL_IID_PREFETCHSTATUS, &prefetch_iface) != SL_RESULT_SUCCESS) goto fail;
    if ((*prefetch_iface)->SetCallbackEventsMask(prefetch_iface, SL_PREFETCHEVENT_STATUSCHANGE|SL_PREFETCHEVENT_FILLLEVELCHANGE) != SL_RESULT_SUCCESS) goto fail;
    if ((*prefetch_iface)->RegisterCallback(prefetch_iface, cb_prefetch, this) != SL_RESULT_SUCCESS) goto fail;
    (*prefetch_iface)->SetFillUpdatePeriod(prefetch_iface, 50);
    // playback event
    if ((*m_play_iface)->SetCallbackEventsMask(m_play_iface, SL_PLAYEVENT_HEADATEND) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_play_iface)->RegisterCallback(m_play_iface, cb_playback, this) != SL_RESULT_SUCCESS) goto fail;
    // preload media
    (*m_play_iface)->SetPlayState(m_play_iface, SL_PLAYSTATE_PAUSED);
    return true;
fail:
    unload();
    return false;
#elif defined(PLATFORM_MAC)
    NSString *path = [NSString stringWithUTF8String:(engine->m_psd->res_path + name).c_str()];
    NSSound *sound = [[NSSound alloc] initWithContentsOfFile:path byReference:YES];
    if (!sound) return false;
    AudioPlayerDelegate * d = [[AudioPlayerDelegate alloc] init];
    d.player = this;
    sound.delegate = d;
    m_player = (__bridge_retained void*)sound;
    m_delegate = (__bridge_retained void*)d;
    // assume its loaded
    m_loaded = true;
    ev_status.call(this, Status::Loaded);
    return true;
#elif defined(PLATFORM_IOS)
    NSString *path = [NSString stringWithUTF8String:(engine->m_psd->res_path + name).c_str()];
    NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:path];
    AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:nil];
    if (!player) return false;
    AudioPlayerDelegate * d = [[AudioPlayerDelegate alloc] init];
    d.player = this;
    player.delegate = d;
    m_player = (__bridge_retained void*)player;
    m_delegate = (__bridge_retained void*)d;
    // assume its loaded
    m_loaded = true;
    ev_status.call(this, Status::Loaded);
    return true;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void AudioPlayer::unload() {
    stop();
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (m_source) {
        m_source->Shutdown();
        m_source->Release();
        m_source = nullptr;
    }
    if (m_session && !m_closed) {
        m_closed = true;
        m_session->Shutdown();
        // m_session->Release();
        // m_session = nullptr;
    }
#elif defined(PLATFORM_ANDROID)
    if (m_player) {
        if (m_play_iface) {
            (*m_play_iface)->SetPlayState(m_play_iface, SL_PLAYSTATE_STOPPED);
        }
        (*m_player)->Destroy(m_player);
        m_player = nullptr;
    }
    m_play_iface = nullptr;
    m_seek_iface = nullptr;
    m_vol_iface = nullptr;
#elif defined(PLATFORM_MAC)
    if (m_delegate) {
        AudioPlayerDelegate* d = (__bridge_transfer AudioPlayerDelegate*)m_delegate;
        (void)d;
        m_delegate = nullptr;
    }
    if (m_player) {
        NSSound* player = (__bridge_transfer NSSound*)m_player;
        m_player = nullptr;
        [player stop];
    }
#elif defined(PLATFORM_IOS)
    if (m_delegate) {
        AudioPlayerDelegate* d = (__bridge_transfer AudioPlayerDelegate*)m_delegate;
        (void)d;
        m_delegate = nullptr;
    }
    if (m_player) {
        AVAudioPlayer* player = (__bridge_transfer AVAudioPlayer*)m_player;
        m_player = nullptr;
        [player stop];
    }
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool AudioPlayer::play() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    PROPVARIANT varStart;
    if (!m_session) return false;
    PropVariantInit(&varStart);
    if (FAILED(m_session->Start(&GUID_NULL, &varStart))) return false;
    PropVariantClear(&varStart);
    m_status = Status::Playing;
    ev_status.call(this, m_status);
    return true;
#elif defined(PLATFORM_ANDROID)
    if (!m_play_iface) return false;
    if ((*m_play_iface)->SetPlayState(m_play_iface, SL_PLAYSTATE_PLAYING) == SL_RESULT_SUCCESS) {
        ev_status.call(this, Status::Playing);
        return true;
    } return false;
#elif defined(PLATFORM_MAC)
    if (!m_player) return false;
    NSSound* player = (__bridge NSSound*)m_player;
    if ([player play]) {
        ev_status.call(this, Status::Playing);
        return true;
    } return false;
#elif defined(PLATFORM_IOS)
    if (!m_player) return false;
    AVAudioPlayer* player = (__bridge AVAudioPlayer*)m_player;
    if ([player play]) {
        ev_status.call(this, Status::Playing);
        return true;
    } return false;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool AudioPlayer::pause() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (!m_session) return false;
    if (FAILED(m_session->Pause())) return false;
    m_status = Status::Paused;
    ev_status.call(this, m_status);
    return true;
#elif defined(PLATFORM_ANDROID)
    if (!m_play_iface) return false;
    if ((*m_play_iface)->SetPlayState(m_play_iface, SL_PLAYSTATE_PAUSED) == SL_RESULT_SUCCESS) {
        ev_status.call(this, Status::Paused);
        return true;
    } return false;
#elif defined(PLATFORM_MAC)
    if (!m_player) return false;
    NSSound* player = (__bridge NSSound*)m_player;
    if ([player pause]) {
        ev_status.call(this, Status::Paused);
        return true;
    } return false;
#elif defined(PLATFORM_IOS)
    if (!m_player) return false;
    AVAudioPlayer* player = (__bridge AVAudioPlayer*)m_player;
    [player pause];
    ev_status.call(this, Status::Paused);
    return true;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool AudioPlayer::stop() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (!m_session) return false;
    if (FAILED(m_session->Stop())) return false;
    m_status = Status::Paused;
    ev_status.call(this, m_status);
    return true;
#elif defined(PLATFORM_ANDROID)
    if (!m_play_iface) return false;
    if ((*m_play_iface)->SetPlayState(m_play_iface, SL_PLAYSTATE_STOPPED) == SL_RESULT_SUCCESS) {
        ev_status.call(this, Status::Paused);
        return true;
    } return false;
#elif defined(PLATFORM_MAC)
    if (!m_player) return false;
    NSSound* player = (__bridge NSSound*)m_player;
    if ([player stop]) {
        ev_status.call(this, Status::Paused);
        return true;
    } return false;
#elif defined(PLATFORM_IOS)
    if (!m_player) return false;
    AVAudioPlayer* player = (__bridge AVAudioPlayer*)m_player;
    [player stop];
    ev_status.call(this, Status::Paused);
    return true;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool AudioPlayer::is_playing() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    return m_status == Status::Playing;
#elif defined(PLATFORM_ANDROID)
    SLuint32 state;
    if (!m_play_iface) return false;
    if ((*m_play_iface)->GetPlayState(m_play_iface, &state) == SL_RESULT_SUCCESS) {
        return state == SL_PLAYSTATE_PLAYING;
    } return false;
#elif defined(PLATFORM_MAC)
    if (!m_player) return false;
    NSSound* player = (__bridge NSSound*)m_player;
    return [player isPlaying];
#elif defined(PLATFORM_IOS)
    if (!m_player) return false;
    AVAudioPlayer* player = (__bridge AVAudioPlayer*)m_player;
    return [player isPlaying];
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
unsigned long AudioPlayer::duration() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    return m_duration;
#elif defined(PLATFORM_ANDROID)
    SLmillisecond ms;
    if (!m_play_iface) return 0;
    if ((*m_play_iface)->GetDuration(m_play_iface, &ms) == SL_RESULT_SUCCESS) {
        return (unsigned long)ms;
    } return 0;
#elif defined(PLATFORM_MAC)
    if (!m_player) return false;
    NSSound* player = (__bridge NSSound*)m_player;
    return 1000UL * [player duration];
#elif defined(PLATFORM_IOS)
    if (!m_player) return false;
    AVAudioPlayer* player = (__bridge AVAudioPlayer*)m_player;
    return 1000UL * [player duration];
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
// AudioEngine
// ----------------------------------------------------------------------------
AudioEngine::AudioEngine() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // NOTHING
#elif defined(PLATFORM_ANDROID)
    m_engine = nullptr;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // NOTHING
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
AudioEngine::~AudioEngine() {
    fini();
}
// ----------------------------------------------------------------------------
bool AudioEngine::init(const PlatformSpecificData* psd) {
    m_psd = psd;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (MFStartup(MF_VERSION) != S_OK) return false;
    return true;
#elif defined(PLATFORM_ANDROID)
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    if (slCreateEngine(&m_instance, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_instance)->Realize(m_instance, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_instance)->GetInterface(m_instance, SL_IID_ENGINE, &m_engine) != SL_RESULT_SUCCESS) goto fail;
    // create output mix, with environmental reverb specified as a non-required interface
    if ((*m_engine)->CreateOutputMix(m_engine, &m_outmix, 1, ids, req) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_outmix)->Realize(m_outmix, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) goto fail;
    return true;
fail:
    fini();
    return false;
#elif defined(PLATFORM_MAC)
    return true;
#elif defined(PLATFORM_IOS)
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
    [[AVAudioSession sharedInstance] setActive: YES error: nil];
    return true;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void AudioEngine::fini() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    //MFShutdown();
#elif defined(PLATFORM_ANDROID)
    if (m_instance) {
        (*m_instance)->Destroy(m_instance);
        m_instance = nullptr;
    }
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // NOTHING
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void AudioEngine::pause() {
}
// ----------------------------------------------------------------------------
void AudioEngine::resume() {
}
// ----------------------------------------------------------------------------
AudioPlayer* AudioEngine::create_player(const std::string& uri) {
    AudioPlayer* player = new AudioPlayer();
    if (!player->load(this, uri)) {
        delete player;
        return nullptr;
    } return player;
}
// ----------------------------------------------------------------------------
