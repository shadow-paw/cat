#ifndef __CAT_UTIL_UNICODE_H__
#define __CAT_UTIL_UNICODE_H__

#include <string>
#include <functional>
#include "cat_platform.h"

namespace cat {
// ----------------------------------------------------------------------------
class StringUtil {
public:
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    static std::basic_string<TCHAR> make_tstring(const char* s);
    static std::basic_string<TCHAR> make_tstring(const std::string& s);
    static std::string make_string(const TCHAR* s);
    static void tstrings_each(const TCHAR* s, std::function<bool(const std::string& s)> cb);
#endif
    static std::string trim(const std::string& s);
};
// ----------------------------------------------------------------------------
} // namespace cat
  // ----------------------------------------------------------------------------

#endif // __CAT_UTIL_UNICODE_H__
