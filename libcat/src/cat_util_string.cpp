#include "cat_util_string.h"

using namespace cat;

// ----------------------------------------------------------------------------
std::string StringUtil::trim(const std::string& s) {
    auto start = s.find_first_not_of(' ');
    auto end = s.find_last_not_of(' ');
    return s.substr(start, end-start+1);
}
// ----------------------------------------------------------------------------
// Windows
// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
// ----------------------------------------------------------------------------
std::basic_string<TCHAR> StringUtil::make_tstring(const char*s) {
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
std::basic_string<TCHAR> StringUtil::make_tstring(const std::string& s) {
    return make_tstring(s.c_str());
}
// ----------------------------------------------------------------------------
std::string StringUtil::make_string(const TCHAR* s) {
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
void StringUtil::tstrings_each(const TCHAR* s, std::function<bool(const std::string& s)> cb) {
    std::string rez;
    if (!s || s[0]==0) return;
    int len = WideCharToMultiByte(CP_UTF8, 0, s, -1, nullptr, 0, NULL, NULL);
    if (len > 0) {
        rez.resize(len);
        len = WideCharToMultiByte(CP_UTF8, 0, s, -1, &rez[0], len, NULL, NULL);
        rez.resize(len == 0 ? 0 : len - 1);
        if (!cb(std::move(rez))) return;
        if (len>0) return tstrings_each(s + len, cb);
    }
}
// ----------------------------------------------------------------------------
#endif
