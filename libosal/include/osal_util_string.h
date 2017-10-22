#ifndef __OSAL_UTIL_UNICODE_H__
#define __OSAL_UTIL_UNICODE_H__

#include <string>
#include "osal_platform.h"

namespace osal {
// ----------------------------------------------------------------------------
class StringUtil {
public:
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    static std::basic_string<TCHAR> string2tchar(const char* s);
    static std::basic_string<TCHAR> string2tchar(const std::string& s);
    static std::string tchar2string(const TCHAR* s);
#endif
};
// ----------------------------------------------------------------------------
} // namespace osal
  // ----------------------------------------------------------------------------

#endif // __OSAL_UTIL_UNICODE_H__
