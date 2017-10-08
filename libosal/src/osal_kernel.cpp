#include "osal_kernel.h"
#include <chrono>

using namespace osal;

// ----------------------------------------------------------------------------
Kernel::Kernel() {
}
// ----------------------------------------------------------------------------
Kernel::~Kernel() {
    fini();
}
// ----------------------------------------------------------------------------
bool Kernel::init(const PlatformSpecificData& psd) {
    m_psd = psd;
    if (!m_renderer.init()) goto fail;
    return true;
fail:
    fini();
    return false;
}
// ----------------------------------------------------------------------------
void Kernel::fini() {
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
Platform Kernel::platform() const {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    return Platform::Windows;
#elif defined(PLATFORM_MACOSX)
    return Platform::Mac;
#elif defined(PLATFORM_IOS)
    return Platform::IOS;
#elif defined(PLATFORM_ANDROID)
    return Platform::Android;
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
// Kernel Signals
// ----------------------------------------------------------------------------
void Kernel::startup() {
    cb_startup(now());
    m_renderer.dirty();
}
// ----------------------------------------------------------------------------
void Kernel::shutdown() {
    cb_shutdown(now());
}
// ----------------------------------------------------------------------------
void Kernel::context_lost() {
    cb_context_lost();
    m_renderer.context_lost();
}
// ----------------------------------------------------------------------------
bool Kernel::context_restored() {
    m_renderer.context_restored();
    cb_context_restored();
    return true;
}
// ----------------------------------------------------------------------------
void Kernel::pause() {
}
// ----------------------------------------------------------------------------
void Kernel::resume() {
}
// ----------------------------------------------------------------------------
void Kernel::resize(int width, int height) {
    m_renderer.resize(width, height);
    cb_resize(width, height);
}
// ----------------------------------------------------------------------------
bool Kernel::touch(TouchEvent ev) {
    m_renderer.dirty();
    return false;
}
// ----------------------------------------------------------------------------
bool Kernel::timer() {
    return false;
}
// ----------------------------------------------------------------------------
void Kernel::render() {
    Timestamp t = now();
    m_renderer.begin_render();
    cb_render(&m_renderer, t);
    m_renderer.end_render();
}
// ----------------------------------------------------------------------------
