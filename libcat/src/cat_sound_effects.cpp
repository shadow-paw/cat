#include "cat_sound_effects.h"

using namespace cat;

// ----------------------------------------------------------------------------
SoundEffects::SoundEffects() {
    m_psd = nullptr;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    m_soundpool = nullptr;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
SoundEffects::~SoundEffects() {
}
// ----------------------------------------------------------------------------
bool SoundEffects::init(const PlatformSpecificData* psd) {
    m_psd = psd;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    jobject jpool = nullptr;
    if (jni.ANDROID_SDK_INT() >= 21) {
        const int USAGE_GAME = 14;
        const int CONTENT_TYPE_MUSIC = 2;
        // jattrs_builder = new AudioAttributes.Builder();
        jobject jattrs_builder = jni.NewObject("android/media/AudioAttributes$Builder", "()V");
        // jattrs_builder.setUsage(AudioAttributes.USAGE_MEDIA);
        jni.CallObjectMethod(jattrs_builder, "setUsage", "(I)Landroid/media/AudioAttributes$Builder;", USAGE_GAME);
        // jattrs_builder.setContentType(AudioAttributes.CONTENT_TYPE_MUSIC);
        jni.CallObjectMethod(jattrs_builder, "setContentType", "(I)Landroid/media/AudioAttributes$Builder;", CONTENT_TYPE_MUSIC);
        // jattrs = jattrs_builder.build();
        jobject jattrs = jni.CallObjectMethod(jattrs_builder, "build", "()Landroid/media/AudioAttributes;");

        // jsp_builder = new SoundPool.Builder();
        jobject jsp_builder = jni.NewObject("android/media/SoundPool$Builder", "()V");
        // jsp_builder.setMaxStreams(MAX_STREAM);
        jni.CallObjectMethod(jsp_builder, "setMaxStreams", "(I)Landroid/media/SoundPool$Builder;", MAX_STREAM);
        // jsp_builder.setAudioAttributes(attrs);
        jni.CallObjectMethod(jsp_builder, "setAudioAttributes", "(Landroid/media/AudioAttributes;)Landroid/media/SoundPool$Builder;", jattrs);
        // jpool = jsp_builder.build();
        jpool = jni.CallObjectMethod(jsp_builder, "build", "()Landroid/media/SoundPool;");
    } else {
        // fallback
        const int STREAM_MUSIC = 3;
        jpool = jni.NewObject("android/media/SoundPool", "(III)V", MAX_STREAM, STREAM_MUSIC, 0);
    }
    if (jpool == nullptr) return false;
    m_soundpool = jni.NewGlobalRef(jpool);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
#else
    #error Not Implemented!
#endif
    return true;
}
// ----------------------------------------------------------------------------
void SoundEffects::fini() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    if (m_soundpool) {
        JNIHelper jni;
        jni.DeleteGlobalRef(m_soundpool);
        m_soundpool = nullptr;
    }
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void SoundEffects::pause() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    jni.CallVoidMethod(m_soundpool, "autoPause", nullptr);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void SoundEffects::resume() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    jni.CallVoidMethod(m_soundpool, "autoResume", nullptr);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
const SoundEffect* SoundEffects::load(const std::string& name) {
    auto cached = m_sounds.find(name);
    if (cached != m_sounds.end()) {
        auto& pair = cached->second;
        pair.second++;
        return pair.first;
    }
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
    return nullptr;
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    // fd = assetsManager.openFd(name[1]); 
    const jint LOAD_PRIORITY_NORMAL = 1;
    if (name.compare("/assets/") != 0) return nullptr;
    jobject jafd = jni.CallObjectMethod(m_psd->asset_manager, "openFd", "(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;", jni.NewStringUTF(name.substr(8)));
    if (jafd == nullptr) return nullptr;
    jint sound_id = jni.CallIntMethod(m_soundpool, "load", "(Ljava/io/FileDescriptor;JJI)I",
        jni.CallObjectMethod(jafd, "getFileDescriptor", "()Ljava/io/FileDescriptor;"),
        jni.CallLongMethod(jafd, "getStartOffset", nullptr),
        jni.CallLongMethod(jafd, "getLength", nullptr),
        LOAD_PRIORITY_NORMAL);
    // fd.close()
    jni.CallVoidMethod(jafd, "close", nullptr);
    if (sound_id == 0) return nullptr;
    SoundEffect* sound = new SoundEffect();
    sound->name = name;
    sound->sound_id = sound_id;
    m_sounds.emplace(name, std::make_pair(sound, 1));
    return sound;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
    return nullptr;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool SoundEffects::unload(const SoundEffect* sound) {
    if (sound == nullptr) return false;
    for (auto it = m_sounds.begin(); it != m_sounds.end(); ++it) {
        auto& pair = it->second;
        if (pair.first != sound) continue;
        pair.second--;
        if (pair.second == 0) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
            // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
            JNIHelper jni;
            jni.CallBooleanMethod(m_soundpool, "unload", "(I)Z", pair.first->sound_id);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
            // TODO: Not Implemented
#else
    #error Not Implemented!
#endif
            delete pair.first;
            m_sounds.erase(it);
        } return true;
    }
    return false;
}
// ----------------------------------------------------------------------------
void SoundEffects::play(const SoundEffect* sound, float left_volume, float right_volume) {
    if (!sound) return;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Not Implemented
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    jni.CallIntMethod(m_soundpool, "play", "(IFFIIF)I", sound->sound_id, left_volume, right_volume, 1, 0, 1.0f);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // TODO: Not Implemented
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
