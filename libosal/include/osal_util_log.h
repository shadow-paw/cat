#ifndef __OSAL_UTIL_LOG_H__
#define __OSAL_UTIL_LOG_H__

namespace osal {
// ----------------------------------------------------------------------------
class Logger {
public:
    static void d(const char* tag, const char* fmt, ...);
    static void e(const char* tag, const char* fmt, ...);
};
// ----------------------------------------------------------------------------
} // namespace osal
// ----------------------------------------------------------------------------

#endif // __OSAL_UTIL_LOG_H__
