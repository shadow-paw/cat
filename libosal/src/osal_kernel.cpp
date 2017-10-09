#include "osal_kernel.h"
#include "osal_util_log.h"

#include <chrono>

using namespace osal;

// ----------------------------------------------------------------------------
Kernel::Kernel() : m_res(&m_vfs) {
}
// ----------------------------------------------------------------------------
Kernel::~Kernel() {
    fini();
}
// ----------------------------------------------------------------------------
bool Kernel::init(const PlatformSpecificData& psd) {
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
    m_res.fini();
    m_renderer.fini();
}
// ----------------------------------------------------------------------------
// Useful Functions
// ----------------------------------------------------------------------------
Timestamp Kernel::now() const {
    return (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
}
// ----------------------------------------------------------------------------
// Kernel Signals
// ----------------------------------------------------------------------------
void Kernel::startup() {
}
// ----------------------------------------------------------------------------
void Kernel::shutdown() {
}
// ----------------------------------------------------------------------------
void Kernel::context_lost() {
    for (auto& app : m_apps) {
        app->cb_context_lost();
    }
    m_res.context_lost();
    m_renderer.context_lost();
}
// ----------------------------------------------------------------------------
bool Kernel::context_restored() {
    m_renderer.context_restored();
    m_res.context_restored();
    for (auto& app : m_apps) {
        app->cb_context_restored();
    } return true;
}
// ----------------------------------------------------------------------------
void Kernel::pause() {
    for (auto& app : m_apps) {
        app->cb_pause();
    }
}
// ----------------------------------------------------------------------------
void Kernel::resume() {
    for (auto& app : m_apps) {
        app->cb_resume();
    }
}
// ----------------------------------------------------------------------------
void Kernel::resize(int width, int height) {
    m_renderer.resize(width, height);
    for (auto& app : m_apps) {
        app->cb_resize(width, height);
    }
}
// ----------------------------------------------------------------------------
bool Kernel::touch(TouchEvent ev) {
    m_renderer.dirty();
    return false;
}
// ----------------------------------------------------------------------------
bool Kernel::timer() {
    // Remove exited app
    for (auto it = m_apps.begin(); it!=m_apps.end(); ) {
        if ((*it)->m_running) {
            ++it;
        } else {
            it = m_apps.erase(it);
        }
    }

    return false;
}
// ----------------------------------------------------------------------------
void Kernel::render() {
    Timestamp t = now();
    m_renderer.begin_render();
    for (auto& app : m_apps) {
        app->cb_render(&m_renderer, t);
    }
    m_renderer.end_render();
}
// ----------------------------------------------------------------------------
// Application
// ----------------------------------------------------------------------------
bool Kernel::run(Application* app) {
    app->m_kernelapi = this;
    app->m_running = true;
    m_apps.push_back(std::unique_ptr<Application>(app));
    if (m_renderer.ready()) {
        app->cb_context_restored();
    }
    app->cb_startup(now());
    return true;
}
// ----------------------------------------------------------------------------
