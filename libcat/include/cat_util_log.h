#ifndef __CAT_UTIL_LOG_H__
#define __CAT_UTIL_LOG_H__

namespace cat {
// ----------------------------------------------------------------------------
class Logger {
public:
    //! Debug log, only appear in debug build
    //! \param tag Category for the log message
    //! \param fmt printf style format message
    static void d(const char* tag, const char* fmt, ...);
    //! Error log
    //! \param tag Category for the log message
    //! \param fmt printf style format message
    static void e(const char* tag, const char* fmt, ...);
};
// ----------------------------------------------------------------------------
} // namespace cat
// ----------------------------------------------------------------------------

#endif // __CAT_UTIL_LOG_H__
