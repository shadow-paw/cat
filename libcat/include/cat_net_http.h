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
#include <forward_list>
#include <mutex>
#include "cat_storage_buffer.h"

namespace cat {
class NetService;
// ----------------------------------------------------------------------------
class HttpManager {
friend class cat::NetService;
public:
    HttpManager();
    ~HttpManager();

    bool http(const char* url, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)>);
private:
    // called from NetService
    void pause();
    void resume();
    void poll();
private:
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    class Session {
    public:
        HttpManager* manager;
        HINTERNET    handle;
        bool         success;
        uint8_t*     buffer;
        size_t       buflen;
        std::function<void(bool, const uint8_t*, size_t)> cb;

        Session() { buffer = nullptr; buflen = 0; }
        ~Session() { if (buffer) delete buffer; }
    };
    HINTERNET  m_internet;
    std::mutex m_mutex;
    std::forward_list<Session> m_sessions;

    static void CALLBACK cb_inet_status(HINTERNET handle, DWORD_PTR ud, DWORD status, LPVOID info, DWORD infolen);
    void cb_inet_status(Session* session, HINTERNET handle, DWORD status, LPVOID info, DWORD infolen);
#else
    #error Not Implemented!
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_NET_HTTP_H__
