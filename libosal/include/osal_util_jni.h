#ifndef __OSAL_UTIL_JNI_H__
#define __OSAL_UTIL_JNI_H__
#if defined (PLATFORM_ANDROID)

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

    // Class & Method Resolver
    // ------------------------------------------------------------------------
    jclass FindClass(const char* name) {
        return (jclass)m_env->FindClass(name);
    }
    jmethodID GetStaticMethodID(jclass clazz, const char* method, const char* sig = nullptr) {
        return m_env->GetStaticMethodID(clazz, method, sig ? sig : "()V");
    }
    jmethodID GetMethodID(jclass clazz, const char* method, const char* sig = nullptr) {
        return m_env->GetMethodID(clazz, method, sig?sig:"()V");
    }
    // Reference
    // ------------------------------------------------------------------------
    jobject NewGlobalRef(jobject o) {
        return m_env->NewGlobalRef(o);
    }
    void DeleteGlobalRef(jobject o) {
        if (o) m_env->DeleteGlobalRef(o);
    }
    // String
    // ------------------------------------------------------------------------
    jstring NewStringUTF(const char* utf) {
        return m_env->NewStringUTF(utf);
    }
    jsize GetStringLength(jstring s) {
        return m_env->GetStringLength(s);
    }
    const char* GetStringUTFChars(jstring s) {
        return m_env->GetStringUTFChars(s, NULL);
    }
    // New Object
    // ------------------------------------------------------------------------
    template <class... ARG>
    jobject NewObject(const char* clazz, const char* sig, ARG... arg) {
        jclass    jclazz = (jclass)m_env->FindClass(clazz);
        jmethodID jctor = m_env->GetMethodID(jclazz, "<init>", sig);
        return m_env->NewObject(jclazz, jctor, arg...);
    }
    template <class... ARG>
    jobject NewObject(jclass clazz, const char* sig, ARG... arg) {
        jmethodID jctor = m_env->GetMethodID(clazz, "<init>", sig);
        return m_env->NewObject(clazz, jctor, arg...);
    }
    // Calling Method
    // ------------------------------------------------------------------------
    template <class... ARG>
    void CallVoidMethod(jobject o, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        jmethodID jmethod = m_env->GetMethodID(jclazz, method, sig);
        m_env->CallVoidMethod(o, jmethod, arg...);
    }
    template <class... ARG>
    void CallVoidMethod(jobject o, jmethodID method, ARG... arg) {
        m_env->CallVoidMethod(o, method, arg...);
    }
    template <class... ARG>
    jint CallIntMethod(jobject o, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        jmethodID jmethod = m_env->GetMethodID(jclazz, method, sig);
        return m_env->CallIntMethod(o, jmethod, arg...);
    }
    template <class... ARG>
    jint CallIntMethod(jobject o, jmethodID method, ARG... arg) {
        return m_env->CallIntMethod(o, method, arg...);
    }
    template <class... ARG>
    jobject CallObjectMethod(jobject o, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        jmethodID jmethod = m_env->GetMethodID(jclazz, method, sig);
        return m_env->CallObjectMethod(o, jmethod, arg...);
    }
    template <class... ARG>
    jobject CallObjectMethod(jobject o, jmethodID method, ARG... arg) {
        return m_env->CallObjectMethod(o, method, arg...);
    }
    // Calling Static Method
    // ------------------------------------------------------------------------
    template <class... ARG>
    void CallStaticVoidMethod(const char* clazz, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->FindClass(clazz);
        jmethodID jmethod = m_env->GetStaticMethodID(jclazz, method, sig);
        m_env->CallStaticVoidMethod(jclazz, jmethod, arg...);
    }
    template <class... ARG>
    void CallStaticVoidMethod(jclass clazz, jmethodID method, ARG... arg) {
        m_env->CallStaticVoidMethod(clazz, method, arg...);
    }
    template <class... ARG>
    jobject CallStaticObjectMethod(const char* clazz, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->FindClass(clazz);
        jmethodID jmethod = m_env->GetStaticMethodID(jclazz, method, sig);
        return m_env->CallStaticObjectMethod(jclazz, jmethod, arg...);
    }
    template <class... ARG>
    jobject CallStaticObjectMethod(jclass clazz, jmethodID method, ARG... arg) {
        return m_env->CallStaticObjectMethod(clazz, method, arg...);
    }
    // Field
    // ------------------------------------------------------------------------
    void SetIntField(jobject o, const char* field, jint v) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        m_env->SetIntField(o, m_env->GetFieldID(jclazz, field, "I"), v);
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
