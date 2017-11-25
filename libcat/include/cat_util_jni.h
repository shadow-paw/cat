#ifndef __CAT_UTIL_JNI_H__
#define __CAT_UTIL_JNI_H__
#if defined (PLATFORM_ANDROID)

#include <jni.h>
#include <string>

namespace cat {
// ----------------------------------------------------------------------------
class JNIHelper {
public:
    //! Initialize internal static data
    //! \param env JNI environment
    //! \return true if success, false if failed and no side-effect
    static bool init(JNIEnv* env);
    //! Finalize and cleanup, cannot use JNIHelper afterward
    static void fini();

    //! Create an instanct of JNI environment for the calling thread, attach to JVM if not already
    JNIHelper();
    ~JNIHelper();
    //! Get the low level java environment
    //! Note that you should use the helper functions provided by this class
    JNIEnv* env() { return m_env; }

    // Class & Method Resolver
    // ------------------------------------------------------------------------
    //! Resolve a class with name
    //! note that you may want to keep a global reference if you plan to cache and reuse later
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param name class name in form of "com/company/MyObject" or "com/company/MyObject$NestedClass"
    //! \return jclass handle
    jclass FindClass(const char* name) {
        return (jclass)m_env->FindClass(name);
    }
    //! Resolve a statc method of a class
    //! note that you may want to keep a global reference if you plan to cache and reuse later
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz class handle returned by FindClass
    //! \param method method name, e.g. "charAt"
    //! \param sig method signature, e.g. "(I)c"
    //! \return jmethod handle
    //! \sa FindClass
    jmethodID GetStaticMethodID(jclass clazz, const char* method, const char* sig = nullptr) {
        return m_env->GetStaticMethodID(clazz, method, sig ? sig : "()V");
    }
    //! Resolve a method of a class
    //! note that you may want to keep a global reference if you plan to cache and reuse later
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz class handle returned by FindClass
    //! \param method method name, e.g. "charAt"
    //! \param sig method signature, e.g. "(I)c"
    //! \return jmethod handle
    //! \sa FindClass
    jmethodID GetMethodID(jclass clazz, const char* method, const char* sig = nullptr) {
        return m_env->GetMethodID(clazz, method, sig?sig:"()V");
    }
    // Reference
    // ------------------------------------------------------------------------
    //! Retain a global reference
    //! \param o object to make reference
    //! \return global reference to the object that can be used later
    jobject NewGlobalRef(jobject o) {
        return m_env->NewGlobalRef(o);
    }
    //! Release a global reference
    //! \param o an global reference obtained from NewGlobalRef
    //! \sa NewGlobalRef
    void DeleteGlobalRef(jobject o) {
        if (o) m_env->DeleteGlobalRef(o);
    }
    // String
    // ------------------------------------------------------------------------
    //! Create a java compatible string (java.lang.String)
    //! \param utf8 utf8 null-terminated string
    //! \return java string object
    jstring NewStringUTF(const char* utf8) {
        return m_env->NewStringUTF(utf8);
    }
    //! Create a java compatible string (java.lang.String)
    //! \param utf8 utf8 string
    //! \return java string object
    jstring NewStringUTF(const std::string& utf8) {
        return m_env->NewStringUTF(utf8.c_str());
    }
    //! Get number of unicode character in the string
    //! \param s java string
    //! \return number of unicode characters
    jsize GetStringLength(jstring s) {
        return m_env->GetStringLength(s);
    }
    //! Convert java string to c++ string
    //! \param s java string
    //! \return c++ string
    std::string GetStringUTFChars(jstring s) {
        const char* cstr = m_env->GetStringUTFChars(s, NULL);
        std::string result(cstr);
        m_env->ReleaseStringUTFChars(s, cstr);
        return result;
    }
    // Byte Array
    // ------------------------------------------------------------------------
    //! Create java byte array (byte[])
    //! \param size number of byte to allocate
    //! \return java byte array
    jbyteArray NewByteArray(size_t len) {
        return m_env->NewByteArray(len);
    }
    //! Create java byte array (byte[]) and copy memory region to it
    //! \param mem memory to copy
    //! \param size number of byte to allocate and copy
    //! \return java byte array
    jbyteArray NewByteArray(void* mem, size_t size) {
        jbyteArray jarray = m_env->NewByteArray(size);
        if (jarray && mem) {
            m_env->SetByteArrayRegion(jarray, 0, (jsize)size, reinterpret_cast<jbyte*>(mem));
        } return jarray;
    }
    //! Get number of element in byte array
    //! \param jarray java array object
    //! \return number of element
    size_t GetByteArrayLength(jbyteArray jarray) {
        return (size_t)m_env->GetArrayLength(jarray);
    }
    //! Copy elements to destination memory
    //! i.e. memcpy(dst, &jarray[offset], len*sizeof(byte));
    //! \param jarray java array object
    //! \param dst destination memory buffer
    //! \param offset offset of element to copy
    //! \param len number of element to copy
    void GetByteArrayRegion(jbyteArray jarray, void* dst, size_t offset, size_t len) {
        m_env->GetByteArrayRegion(jarray, (jsize)offset, (jsize)len, reinterpret_cast<jbyte*>(dst));
    }
    // New Object
    // ------------------------------------------------------------------------
    //! Create(new) java object
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz class name, e.g "com/company/MyObject" or "com/company/MyObject$NestedClass"
    //! \param sig constructor signature, e.g. "()V" or "(I)V", nullptr treated as "()V"
    //! \param arg optioanl constructor arguments
    //! \return new java object
    template <class... ARG>
    jobject NewObject(const char* clazz, const char* sig, ARG... arg) {
        jclass    jclazz = (jclass)m_env->FindClass(clazz);
        jmethodID jctor = m_env->GetMethodID(jclazz, "<init>", sig?sig:"()V");
        return m_env->NewObject(jclazz, jctor, arg...);
    }
    //! Create(new) java object
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz java class handle obtained from FindClass
    //! \param sig constructor signature, e.g. "()V" or "(I)V", nullptr treated as "()V"
    //! \param arg optioanl constructor arguments
    //! \return new java object
    //! \sa FindClass
    template <class... ARG>
    jobject NewObject(jclass clazz, const char* sig, ARG... arg) {
        jmethodID jctor = m_env->GetMethodID(clazz, "<init>", sig);
        return m_env->NewObject(clazz, jctor, arg...);
    }
    // Calling Method
    // ------------------------------------------------------------------------
    //! Call java object instance method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param o java object
    //! \param method method name
    //! \param sig method signature, e.g. "()V" or "(I)V", nullptr treated as "()V"
    //! \param arg optioanl arguments
    template <class... ARG>
    void CallVoidMethod(jobject o, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        jmethodID jmethod = m_env->GetMethodID(jclazz, method, sig?sig:"()V");
        m_env->CallVoidMethod(o, jmethod, arg...);
    }
    //! Call java object instance method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param o java object
    //! \param method method id obtained by GetMethodID
    //! \param arg optioanl arguments
    //! \sa GetMethodID
    template <class... ARG>
    void CallVoidMethod(jobject o, jmethodID method, ARG... arg) {
        m_env->CallVoidMethod(o, method, arg...);
    }
    //! Call java object instance method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param o java object
    //! \param method method name
    //! \param sig method signature, e.g. "()I" or "(I)I", nullptr treated as "()I"
    //! \param arg optioanl arguments
    //! \return int return by the method
    template <class... ARG>
    jint CallIntMethod(jobject o, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        jmethodID jmethod = m_env->GetMethodID(jclazz, method, sig?sig:"()I");
        return m_env->CallIntMethod(o, jmethod, arg...);
    }
    //! Call java object instance method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param o java object
    //! \param method method id obtained by GetMethodID
    //! \param arg optioanl arguments
    //! \return int return by the method
    //! \sa GetMethodID
    template <class... ARG>
    jint CallIntMethod(jobject o, jmethodID method, ARG... arg) {
        return m_env->CallIntMethod(o, method, arg...);
    }
    //! Call java object instance method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param o java object
    //! \param method method name
    //! \param sig method signature, e.g. "()Ljava/lang/String;" or "(I)Ljava/lang/String;", nullptr is not allowed
    //! \param arg optioanl arguments
    //! \return object return by the method
    template <class... ARG>
    jobject CallObjectMethod(jobject o, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->GetObjectClass(o);
        jmethodID jmethod = m_env->GetMethodID(jclazz, method, sig);
        return m_env->CallObjectMethod(o, jmethod, arg...);
    }
    //! Call java object instance method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param o java object
    //! \param method method id obtained by GetMethodID
    //! \param arg optioanl arguments
    //! \return object return by the method
    //! \sa GetMethodID
    template <class... ARG>
    jobject CallObjectMethod(jobject o, jmethodID method, ARG... arg) {
        return m_env->CallObjectMethod(o, method, arg...);
    }
    // Calling Static Method
    // ------------------------------------------------------------------------
    //! Call java static method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz class name, e.g "com/company/MyObject" or "com/company/MyObject$NestedClass"
    //! \param method method name
    //! \param sig method signature, e.g. "()V" or "(I)V", nullptr treated as "()V"
    //! \param arg optioanl arguments
    template <class... ARG>
    void CallStaticVoidMethod(const char* clazz, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->FindClass(clazz);
        jmethodID jmethod = m_env->GetStaticMethodID(jclazz, method, sig?sig:"()V");
        m_env->CallStaticVoidMethod(jclazz, jmethod, arg...);
    }
    //! Call java static method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz java class handle obtained from FindClass
    //! \param method method id obtained by GetMethodID
    //! \param arg optioanl arguments
    //! \sa FindClass, GetMethodID
    template <class... ARG>
    void CallStaticVoidMethod(jclass clazz, jmethodID method, ARG... arg) {
        m_env->CallStaticVoidMethod(clazz, method, arg...);
    }
    //! Call java static method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz class name, e.g "com/company/MyObject" or "com/company/MyObject$NestedClass"
    //! \param method method name
    //! \param sig method signature, e.g. "()V" or "(I)V", nullptr treated as "()V"
    //! \param arg optioanl arguments
    //! \return int return by the method
    template <class... ARG>
    jint CallStaticIntMethod(const char* clazz, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->FindClass(clazz);
        jmethodID jmethod = m_env->GetStaticMethodID(jclazz, method, sig ? sig : "()V");
        return m_env->CallStaticIntMethod(jclazz, jmethod, arg...);
    }
    //! Call java static method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz java class handle obtained from FindClass
    //! \param method method id obtained by GetMethodID
    //! \param arg optioanl arguments
    //! \return int return by the method
    //! \sa FindClass, GetMethodID
    template <class... ARG>
    jint CallStaticIntMethod(jclass clazz, jmethodID method, ARG... arg) {
        return m_env->CallStaticIntMethod(clazz, method, arg...);
    }
    //! Call java static method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz class name, e.g "com/company/MyObject" or "com/company/MyObject$NestedClass"
    //! \param method method name
    //! \param sig method signature, e.g. "()Ljava/lang/String;" or "(I)Ljava/lang/String;", nullptr is not allowed
    //! \param arg optioanl arguments
    //! \return object return by the method
    template <class... ARG>
    jobject CallStaticObjectMethod(const char* clazz, const char* method, const char* sig, ARG... arg) {
        jclass jclazz = (jclass)m_env->FindClass(clazz);
        jmethodID jmethod = m_env->GetStaticMethodID(jclazz, method, sig);
        return m_env->CallStaticObjectMethod(jclazz, jmethod, arg...);
    }
    //! Call java static method
    //! See java manual for detail: https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
    //! \param clazz java class handle obtained from FindClass
    //! \param method method id obtained by GetMethodID
    //! \param arg optioanl arguments
    //! \return object return by the method
    //! \sa FindClass, GetMethodID
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
