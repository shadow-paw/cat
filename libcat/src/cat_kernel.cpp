#include "cat_kernel.h"
#include "cat_util_jni.h"
#include "cat_util_log.h"

using namespace cat;

// ----------------------------------------------------------------------------
Kernel::Kernel() : m_res(&m_vfs), m_ui(this) {
    m_resumed = false;
    m_psd = {0};
}
// ----------------------------------------------------------------------------
Kernel::~Kernel() {
    fini();
}
// ----------------------------------------------------------------------------
bool Kernel::init(const PlatformSpecificData& psd) {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64) || defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    m_psd = psd;
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    m_psd.rootview = jni.NewGlobalRef(psd.rootview);
    m_psd.asset_manager = jni.NewGlobalRef(psd.asset_manager);
#else
    #error Not Implemented!
#endif
    if (!m_renderer.init()) goto fail;
    if (!m_res.init()) goto fail;
    // services
    if (!m_ui.init()) goto fail;
    if (!m_time.init()) goto fail;
    if (!m_net.init()) goto fail;
    return true;
fail:
    fini();
    return false;
}
// ----------------------------------------------------------------------------
void Kernel::fini() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    // services
    m_net.fini();
    m_time.fini();
    m_ui.fini();
    // core
    m_res.fini();
    m_renderer.fini();
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64) || defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // NOTHING
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    jni.DeleteGlobalRef(m_psd.rootview); m_psd.rootview = nullptr;
    jni.DeleteGlobalRef(m_psd.asset_manager); m_psd.asset_manager = nullptr;
#else
#error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
// Kernel Signals
// ----------------------------------------------------------------------------
void Kernel::startup() {
    //std::lock_guard<std::mutex> lock(m_bigkernellock);
    resume();
}
// ----------------------------------------------------------------------------
void Kernel::shutdown() {
    pause();
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    // Remove all app
    for (auto it = m_apps.begin(); it != m_apps.end(); ) {
        (*it)->cb_shutdown(m_time.now());
        it = m_apps.erase(it);
    }
}
// ----------------------------------------------------------------------------
void Kernel::context_lost() {
    m_ui.context_lost();
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    for (auto& app : m_apps) {
        app->cb_context_lost();
    }
    m_res.context_lost();
    m_renderer.context_lost();
}
// ----------------------------------------------------------------------------
bool Kernel::context_restored() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    m_renderer.context_restored();
    m_res.context_restored();
    for (auto& app : m_apps) {
        app->cb_context_restored();
    }
    m_ui.context_restored();
    return true;
}
// ----------------------------------------------------------------------------
void Kernel::pause() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    for (auto& app : m_apps) {
        app->cb_pause();
    }
    m_net.pause();
    m_time.pause();
    m_ui.pause();
    m_resumed = false;
}
// ----------------------------------------------------------------------------
void Kernel::resume() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    if (m_resumed) return;
    m_resumed = true;
    m_ui.resume();
    m_time.resume();
    m_net.resume();
    for (auto& app : m_apps) {
        app->cb_resume();
    }
}
// ----------------------------------------------------------------------------
void Kernel::resize(int width, int height) {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    m_renderer.resize(width, height);
    m_ui.resize(width, height);
    for (auto& app : m_apps) {
        app->cb_resize(width, height);
    }
}
// ----------------------------------------------------------------------------
bool Kernel::touch(TouchEvent ev) {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    ev.timestamp = m_time.now();
    m_ui.touch(ev);
    m_renderer.dirty();
    return false;
}
// ----------------------------------------------------------------------------
bool Kernel::timer() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    // Remove exited app
    for (auto it = m_apps.begin(); it!=m_apps.end(); ) {
        if ((*it)->m_running) {
            ++it;
        } else {
            (*it)->cb_shutdown(m_time.now());
            it = m_apps.erase(it);
        }
    }
    m_net.poll();
    if (m_time.timer()) m_renderer.dirty();
    return m_renderer.is_dirty();
}
// ----------------------------------------------------------------------------
void Kernel::render() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    if (!m_renderer.ready()) return;
    auto t = time()->now();
    m_renderer.begin_render();
    for (auto& app : m_apps) {
        app->cb_render(&m_renderer, t);
    }
    m_ui.render(&m_renderer, t);
    m_renderer.end_render();
}
// ----------------------------------------------------------------------------
// Application
// ----------------------------------------------------------------------------
bool Kernel::run(Application* app) {
    app->m_kernel = this;
    app->m_running = true;
    m_apps.push_back(std::unique_ptr<Application>(app));
    if (m_renderer.ready()) {
        app->cb_context_restored();
    }
    app->cb_resize(m_renderer.width(), m_renderer.height());
    app->cb_startup(time()->now());
    if (m_resumed) app->cb_resume();
    return true;
}
// ----------------------------------------------------------------------------
