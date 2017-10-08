#if defined(PLATFORM_IOS)
#include "osal_util_log.h"

using namespace osal::util;

// ----------------------------------------------------------------------------
void Logger::d(const char* tag, const char* fmt, ...) {
    // TODO: NSLog?
}
// ----------------------------------------------------------------------------
void Logger::e(const char* tag, const char* fmt, ...) {
    // TODO: NSLog?
}
// ----------------------------------------------------------------------------
#endif // PLATFORM_IOS
