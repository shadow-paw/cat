#include "osal_util_log.h"
#include <android/log.h>

using namespace osal;

// ----------------------------------------------------------------------------
void Logger::d(const char* tag, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, tag, fmt, va);
    va_end(va);
}
// ----------------------------------------------------------------------------
void Logger::e(const char* tag, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    __android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, va);
    va_end(va);
}
// ----------------------------------------------------------------------------
