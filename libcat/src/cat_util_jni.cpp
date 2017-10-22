#include "cat_util_jni.h"
#if defined(PLATFORM_ANDROID)

using namespace cat;

// ----------------------------------------------------------------------------
JavaVM* JNIHelper::s_vm = nullptr;

// ----------------------------------------------------------------------------
bool JNIHelper::init(JNIEnv* env) {
    env->GetJavaVM(&s_vm);
    return true;
}
// ----------------------------------------------------------------------------
void JNIHelper::fini() {
    s_vm = nullptr;
}
// ----------------------------------------------------------------------------
JNIHelper::JNIHelper() {
    m_attached = false;
    m_env = nullptr;
    if (s_vm) {
        if (s_vm->GetEnv((void**)&m_env, JNI_VERSION_1_6) == JNI_EDETACHED) {
            if (s_vm->AttachCurrentThread(&m_env, NULL) == JNI_OK) {
                m_attached = true;
            }
        }
    }
}
// ----------------------------------------------------------------------------
JNIHelper::~JNIHelper() {
    if (m_env && m_attached && s_vm) s_vm->DetachCurrentThread();
    m_env = nullptr;
    m_attached = false;
}
// ----------------------------------------------------------------------------
#endif // PLATFORM_ANDROID
