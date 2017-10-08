#ifndef __OSAL_PLATFORM_ANDROID_JNI_H__
#define __OSAL_PLATFORM_ANDROID_JNI_H__
#if defined(PLATFORM_ANDROID)

#include <jni.h>

namespace osal {
// ----------------------------------------------------------------------------
class JNIHelper {
public:
    static bool init(JNIEnv* env);
    static void fini();

    JNIHelper();
    ~JNIHelper();

    JNIEnv* env() const { return m_env; }

    jobject NewGlobalRef(jobject o) {
        return m_env->NewGlobalRef(o);
    }
    void DeleteGlobalRef(jobject o) {
        m_env->DeleteGlobalRef(o);
    }
    // JNI Helper
    template <class... ARG>
    jobject new_object(const char* clazz, const char* sig, ARG... arg) {
        jclass    jclazz = (jclass)m_env->FindClass(clazz);
        jmethodID jctor = m_env->GetMethodID(jclazz, "<init>", sig);
        return m_env->NewObject(jclazz, jctor, arg...);
    }
    template <class... ARG>
    void call_void(jobject o, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        jmethodID jmethod = m_env->GetMethodID(jclazz, method, sig);
        m_env->CallVoidMethod(o, jmethod, arg...);
    }
    template <class... ARG>
    jobject call_object(jobject o, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        jmethodID jmethod = m_env->GetMethodID(jclazz, method, sig);
        return m_env->CallObjectMethod(o, jmethod, arg...);
    }

private:
    static JavaVM* s_vm;
    bool m_attached;
    JNIEnv* m_env;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // PLATFORM_ANDROID
#endif // __OSAL_PLATFORM_ANDROID_JNI_H__
