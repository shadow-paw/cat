// ----------------------------------------------------------------------------
// Windows
// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
#include "cat_util_string.h"

using namespace cat;

// ----------------------------------------------------------------------------
std::basic_string<TCHAR> StringUtil::string2tchar(const char*s) {
    std::basic_string<TCHAR> rez;
    if (!s) return rez;
    int len = MultiByteToWideChar(CP_UTF8, 0, s, -1, nullptr, 0);
    if (len == 0) return rez;
    rez.resize(len);
    len = MultiByteToWideChar(CP_UTF8, 0, s, -1, &rez[0], len);
    if (len == 0) {
        rez.resize(0);
        return rez;
    }
    rez.resize(len - 1);
    return rez;
}
// ----------------------------------------------------------------------------
std::basic_string<TCHAR> StringUtil::string2tchar(const std::string& s) {
    return string2tchar(s.c_str());
}
// ----------------------------------------------------------------------------
std::string StringUtil::tchar2string(const TCHAR* s) {
    std::string rez;
    if (!s) return rez;
    int len = WideCharToMultiByte(CP_UTF8, 0, s, -1, nullptr, 0, NULL, NULL);
    if (len == 0) return rez;
    rez.resize(len);
    len = WideCharToMultiByte(CP_UTF8, 0, s, -1, &rez[0], len, NULL, NULL);
    if (len == 0) {
        rez.resize(0);
        return rez;
    }
    rez.resize(len - 1);
    return rez;
}
// ----------------------------------------------------------------------------
#endif
