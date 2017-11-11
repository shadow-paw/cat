#include "cat_kernel.h"
#include "cat_util_log.h"

using namespace cat;

// ----------------------------------------------------------------------------
Kernel::Kernel() : m_res(&m_vfs), m_ui(this) {
}
// ----------------------------------------------------------------------------
Kernel::~Kernel() {
    fini();
}
// ----------------------------------------------------------------------------
bool Kernel::init(const PlatformSpecificData& psd) {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    m_psd = psd;
    if (!m_renderer.init()) goto fail;
    if (!m_res.init()) goto fail;
    return true;
fail:
    fini();
    return false;
}
// ----------------------------------------------------------------------------
void Kernel::fini() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    m_res.fini();
    m_renderer.fini();
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
    } return true;
}
// ----------------------------------------------------------------------------
void Kernel::pause() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    for (auto& app : m_apps) {
        app->cb_pause();
    }
    m_time.pause();
    m_net.pause();
}
// ----------------------------------------------------------------------------
void Kernel::resume() {
    std::lock_guard<std::mutex> lock(m_bigkernellock);
    m_net.resume();
    m_time.resume();
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
    return true;
}
// ----------------------------------------------------------------------------
