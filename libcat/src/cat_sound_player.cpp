#include "cat_sound_player.h"
#if defined(PLATFORM_ANDROID)
  #include <SLES/OpenSLES_Android.h>
  #include <android/asset_manager_jni.h>
#elif defined(PLATFORM_MAC)
  #import <AppKit/AppKit.h>
#elif defined(PLATFORM_IOS)
  #import <AVFoundation/AVFoundation.h>
#endif
#include "cat_util_log.h"

using namespace cat;

// ----------------------------------------------------------------------------
// AudioPlayer delegate
// ----------------------------------------------------------------------------
#if defined(PLATFORM_ANDROID)
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
#elif defined(PLATFORM_MAC)
// ----------------------------------------------------------------------------
@interface AudioPlayerDelegate : NSObject<NSSoundDelegate>
@property (nonatomic) AudioPlayer* player;
@end
@implementation AudioPlayerDelegate
- (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)flag {
    if (self.player) self.player->ev_status.call(self.player, AudioPlayer::Status::Paused);
}
@end
#elif defined(PLATFORM_IOS)
// ----------------------------------------------------------------------------
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
    // TODO: Not Implemented
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
    unload();
}
// ----------------------------------------------------------------------------
bool AudioPlayer::load(AudioEngine* engine, const std::string& name) {
    m_loaded = false;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
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
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
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
    // TODO: Not Implemented
    return false;
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
    // TODO: Not Implemented
    return false;
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
    // TODO: Not Implemented
    return false;
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
    // TODO: Not Implemented
    return false;
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
    // TODO: Not Implemented
    return 0;
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
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    m_engine = nullptr;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
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
    // TODO: Not Implemented
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
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    if (m_instance) {
        (*m_instance)->Destroy(m_instance);
        m_instance = nullptr;
    }
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
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
