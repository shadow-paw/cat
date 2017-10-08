#if defined(PLATFORM_MAC)
#include <stdio.h>
#include <stdarg.h>
#include "osal_util_log.h"

using namespace osal::util;

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
#endif // PLATFORM_MAC