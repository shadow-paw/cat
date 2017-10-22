#ifndef __CAT_UTIL_LOG_H__
#define __CAT_UTIL_LOG_H__

namespace cat {
// ----------------------------------------------------------------------------
class Logger {
public:
    static void d(const char* tag, const char* fmt, ...);
    static void e(const char* tag, const char* fmt, ...);
};
// ----------------------------------------------------------------------------
} // namespace cat
// ----------------------------------------------------------------------------

#endif // __CAT_UTIL_LOG_H__
