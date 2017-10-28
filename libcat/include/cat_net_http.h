#ifndef __CAT_NET_HTTP_H__
#define __CAT_NET_HTTP_H__

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <wininet.h>
    #undef WIN32_LEAN_AND_MEAN
#endif

#include <stdint.h>
#include <functional>
#include <list>
#include <unordered_map>
#include <mutex>
#include "cat_net_type.h"
#include "cat_storage_buffer.h"
#include "cat_util_uniqueid.h"

namespace cat {
class NetService;
// ----------------------------------------------------------------------------
class HttpManager {
friend class cat::NetService;
public:
    HttpManager();
    ~HttpManager();

    HTTP_ID fetch(const char* url, const std::unordered_map<std::string, std::string>* headers, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb);
    HTTP_ID fetch(const char* url, std::function<void(bool, const uint8_t*, size_t)> cb) {
        return fetch(url, nullptr, nullptr, 0, cb);
    }
    HTTP_ID fetch(const char* url, const std::unordered_map<std::string, std::string>* headers, std::function<void(bool, const uint8_t*, size_t)> cb) {
        return fetch(url, headers, nullptr, 0, cb);
    }
    HTTP_ID fetch(const char* url, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb) {
        return fetch(url, nullptr, data, datalen, cb);
    }
    bool cancel(HTTP_ID session_id);

private:
    // called from NetService
    void pause();
    void resume();
    void poll();
private:
    class Session {
    public:
        HTTP_ID    id;
        bool       cancelled, success;
        uint8_t*   buffer;
        size_t     buflen;
        std::function<void(bool, const uint8_t*, size_t)> cb;
        Session();
        Session(Session&& o);
        ~Session();

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
        HINTERNET    hconnect, handle;
#else
    #error Not Implemented!
#endif
    };
    std::mutex m_progress_mutex, m_complete_mutex;
    std::list<Session> m_progress, m_complete;
    UniqueId_r<HTTP_ID> m_unique;

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    HINTERNET  m_internet;
    struct INET_PARAM {
        HttpManager* manager;
        HTTP_ID      http_id;
    };
    static void CALLBACK cb_inet_status(HINTERNET handle, DWORD_PTR ud, DWORD status, LPVOID info, DWORD infolen);
    void cb_inet_status(INET_PARAM* param, HINTERNET handle, DWORD status, LPVOID info, DWORD infolen);
#else
    #error Not Implemented!
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_NET_HTTP_H__
