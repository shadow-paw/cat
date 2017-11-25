#ifndef __CAT_UTIL_UNICODE_H__
#define __CAT_UTIL_UNICODE_H__

#include <string>
#include <functional>
#include "cat_platform.h"

namespace cat {
// ----------------------------------------------------------------------------
//! String utility
class StringUtil {
public:
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    //! Convert utf8 string to Windows TCHAR string
    //! \param s string to convert, can be nullptr
    //! \return basic_string with TCHAR* ts = s.c_str() usable for Windows function.
    static std::basic_string<TCHAR> make_tstring(const char* s);
    //! Convert utf8 string to Windows TCHAR string
    //! \param s string to convert
    //! \return basic_string with TCHAR* ts = s.c_str() usable for Windows function.
    static std::basic_string<TCHAR> make_tstring(const std::string& s);
    //! Convert Windows TCHAR string to utf8 string
    //! \param s string to convert, can be nullptr
    //! \return utf8 string
    static std::string make_string(const TCHAR* s);
    //! Iterate a Windows TCHAR string list
    //! \param s Windows string list in form string\0string\0string\0\0
    //! \param cb Callback for each string, converted to utf8 std::string
    static void tstrings_each(const TCHAR* s, std::function<bool(const std::string& s)> cb);
#endif
    //! Trim space(' ') from begin and end of string
    //! \param s string to trim
    //! \return string with trimmed space
    static std::string trim(const std::string& s);
};
// ----------------------------------------------------------------------------
} // namespace cat
  // ----------------------------------------------------------------------------

#endif // __CAT_UTIL_UNICODE_H__
