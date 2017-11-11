#ifndef __CAT_UTIL_JNI_H__
#define __CAT_UTIL_JNI_H__
#if defined (PLATFORM_ANDROID)

#include <jni.h>
#include <string>

namespace cat {
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
    jstring NewStringUTF(const std::string& utf) {
        return m_env->NewStringUTF(utf.c_str());
    }
    jsize GetStringLength(jstring s) {
        return m_env->GetStringLength(s);
    }
    std::string GetStringUTFChars(jstring s) {
        const char* cstr = m_env->GetStringUTFChars(s, NULL);
        std::string result(cstr);
        m_env->ReleaseStringUTFChars(s, cstr);
        return result;
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
    // Byte Array
    // ------------------------------------------------------------------------
    jbyteArray NewByteArray(size_t len) {
        return m_env->NewByteArray(len);
    }
    jbyteArray NewByteArray(void* buf, size_t len) {
        jbyteArray j_array = m_env->NewByteArray(len);
        if (j_array && buf) {
            m_env->SetByteArrayRegion(j_array, 0, (jsize)len, reinterpret_cast<jbyte*>(buf));
        } return j_array;
    }
    size_t GetByteArrayLength(jbyteArray j_array) {
        return (size_t)m_env->GetArrayLength(j_array);
    }
    void GetByteArrayRegion(jbyteArray j_array, void* dest, size_t offset, size_t len) {
        m_env->GetByteArrayRegion(j_array, (jsize)offset, (jsize)len, reinterpret_cast<jbyte*>(dest));
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
} // namespace cat

#endif // PLATFORM_ANDROID
#endif // __CAT_PLATFORM_ANDROID_JNI_H__
