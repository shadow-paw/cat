#include "cat_sound_player.h"
#if defined(PLATFORM_ANDROID)
  #include <SLES/OpenSLES_Android.h>
  #include <android/asset_manager_jni.h>
#endif

using namespace cat;

// ----------------------------------------------------------------------------
AudioPlayer::AudioPlayer() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    m_player = nullptr;
    m_play_iface = nullptr;
    m_seek_iface = nullptr;
    m_vol_iface = nullptr;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
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
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
    return false;
#elif defined(PLATFORM_ANDROID)
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
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    if ((*engine->m_engine)->CreateAudioPlayer(engine->m_engine, &m_player, &audioSrc, &audioSnk, 3, ids, req) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_player)->Realize(m_player, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) goto fail;
    // query interfaces
    if ((*m_player)->GetInterface(m_player, SL_IID_PLAY, &m_play_iface) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_player)->GetInterface(m_player, SL_IID_SEEK, &m_seek_iface) != SL_RESULT_SUCCESS) goto fail;
    if ((*m_player)->GetInterface(m_player, SL_IID_VOLUME, &m_vol_iface) != SL_RESULT_SUCCESS) goto fail;
    return true;
fail:
    unload();
    return false;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
    return false;
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
        (*m_player)->Destroy(m_player);
        m_player = nullptr;
    }
    m_play_iface = nullptr;
    m_seek_iface = nullptr;
    m_vol_iface = nullptr;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
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
    return (*m_play_iface)->SetPlayState(m_play_iface, SL_PLAYSTATE_PLAYING) == SL_RESULT_SUCCESS;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
    return false;
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
    return (*m_play_iface)->SetPlayState(m_play_iface, SL_PLAYSTATE_PAUSED) == SL_RESULT_SUCCESS;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
    return false;
#else
    #error Not Implemented!
#endif
}
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
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
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
