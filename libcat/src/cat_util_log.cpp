#include "cat_util_log.h"

// ----------------------------------------------------------------------------
// Windows
// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <stdarg.h>
#include <stdio.h>
#include "cat_util_string.h"

using namespace cat;

// ----------------------------------------------------------------------------
void Logger::d(const char* tag, const char* fmt, ...) {
    char buf[8192 + 16];
    va_list va;
    va_start(va, fmt);
    size_t len = vsnprintf(buf, sizeof(buf)-8, fmt, va);
    if (len > sizeof(buf) - 8) len = sizeof(buf) - 8;
    buf[len] = '\n'; buf[len + 1] = 0;
    auto wstr = StringUtil::make_tstring(buf);
    OutputDebugString(wstr.c_str());
    va_end(va);
}
// ----------------------------------------------------------------------------
void Logger::e(const char* tag, const char* fmt, ...) {
    char buf[8192 + 16];
    va_list va;
    va_start(va, fmt);
    size_t len = vsnprintf(buf, sizeof(buf)-8, fmt, va);
    if (len >= sizeof(buf) - 9) len = sizeof(buf) - 9;
    buf[len] = '\n'; buf[len + 1] = 0;
    auto wstr = StringUtil::make_tstring(buf);
    OutputDebugString(wstr.c_str());
    va_end(va);
}
// ----------------------------------------------------------------------------
// MAC
// ----------------------------------------------------------------------------
#elif defined(PLATFORM_MAC)
#include <stdio.h>
#include <stdarg.h>

using namespace cat;

// ----------------------------------------------------------------------------
void Logger::d(const char* tag, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
    printf("\n");
}
// ----------------------------------------------------------------------------
void Logger::e(const char* tag, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
    printf("\n");
}
// ----------------------------------------------------------------------------
// IOS
// ----------------------------------------------------------------------------
#elif defined(PLATFORM_IOS)
#include <stdio.h>
#include <stdarg.h>

using namespace cat;

// ----------------------------------------------------------------------------
void Logger::d(const char* tag, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    printf("\n");
}
// ----------------------------------------------------------------------------
void Logger::e(const char* tag, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    printf("\n");
}
// ----------------------------------------------------------------------------
// Android
// ----------------------------------------------------------------------------
#elif defined(PLATFORM_ANDROID)
#include <android/log.h>

using namespace cat;

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
#else
    #error Not Implemented!
#endif
