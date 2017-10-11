#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <stdarg.h>
#include <stdio.h>
#include "osal_util_log.h"

using namespace osal;

// ----------------------------------------------------------------------------
void Logger::d(const char* tag, const char* fmt, ...) {
    char buf[1024 + 16];
    TCHAR wbuf[1024 + 16];
    va_list va;
    va_start(va, fmt);
    size_t len = vsnprintf(buf, sizeof(buf), fmt, va);
    buf[len] = '\n'; buf[len + 1] = 0;
    MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, 1024);
    OutputDebugString(wbuf);
    va_end(va);
}
// ----------------------------------------------------------------------------
void Logger::e(const char* tag, const char* fmt, ...) {
    char buf[1024 + 16];
    TCHAR wbuf[1024 + 16];
    va_list va;
    va_start(va, fmt);
    size_t len = vsnprintf(buf, sizeof(buf), fmt, va);
    buf[len] = '\n'; buf[len + 1] = 0;
    MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, 1024);
    OutputDebugString(wbuf);
    va_end(va);
}
// ----------------------------------------------------------------------------
