//! NOTE
//! GLSurfaceView.Renderer will call jniContextLost, jniContextRestored and jniRender on GL thread
//! Other functions will be called on main thread, so that we can access Android native UI (e.g. from Widgets/Editbox)
//! To make life simpler we'll just mutex all the calls

#include <jni.h>
#include <mutex>
#include "bootapp.h"

using namespace osal;
using namespace app;

struct OSAL_INSTANCE {
    std::mutex mutex;
    Kernel*    kernel;
    jobject    rootview;
    jobject    asset_manager;
};
extern "C" JNIEXPORT jobject JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniInit(JNIEnv *env, jobject self, jobject assmgr) {
    JNIHelper::init(env);
    OSAL_INSTANCE* data = new OSAL_INSTANCE();
    data->rootview = env->NewGlobalRef(self);
    data->asset_manager = env->NewGlobalRef(assmgr);
    PlatformSpecificData psd;
    psd.rootview = data->rootview;
    data->kernel = new osal::Kernel();
    data->kernel->init(psd);
    data->kernel->vfs()->mount("/assets/", new osal::AssetDriver(data->asset_manager));
    return env->NewDirectByteBuffer((void*)data, sizeof(OSAL_INSTANCE));
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniFini(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    if (data) {
        if (data->kernel) delete data->kernel;
        env->DeleteGlobalRef(data->rootview);
        env->DeleteGlobalRef(data->asset_manager);
        delete data;
    }
    JNIHelper::fini();
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniStartup(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->startup();
    data->kernel->run(new BootApp());
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniShutdown(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->shutdown();
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniContextRestored(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->context_restored();
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniContextLost(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->context_lost();
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniPause(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->pause();
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniResume(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->resume();
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniResize(JNIEnv *env, jobject self, jobject handle, jint width, jint height) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->resize(width, height);
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniRender(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->render();
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniTouch(JNIEnv *env, jobject self, jobject handle, jint type, jint pointerId, jint x, jint y) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    TouchEvent ev;
    switch (type) {
        case 1: ev.type = TouchEvent::EventType::TouchDown; break;
        case 2: ev.type = TouchEvent::EventType::TouchUp; break;
        case 3: ev.type = TouchEvent::EventType::TouchMove; break;
        default: return;
    }
    ev.pointer_id = pointerId;
    ev.x = x;
    ev.y = y;
    ev.button = 1<<0;
    data->kernel->touch(ev);
}
extern "C" JNIEXPORT void JNICALL
Java_com_shadowpaw_osal_glue_OSALView_jniTimer(JNIEnv *env, jobject self, jobject handle) {
    OSAL_INSTANCE* data = (OSAL_INSTANCE*)env->GetDirectBufferAddress(handle);
    std::lock_guard<std::mutex> lock(data->mutex);
    data->kernel->timer();
}
