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
#include <thread>
#include "cat_net_type.h"
#include "cat_util_buffer.h"
#include "cat_util_uniqueid.h"

namespace cat {
class NetService;
// ----------------------------------------------------------------------------
class HttpManager {
friend class cat::NetService;
public:
    HttpManager();
    ~HttpManager();

    HTTP_ID fetch(const std::string& url,
                  std::unordered_multimap<std::string, std::string>&& headers,
                  Buffer&& data,
                  std::function<void(bool, const uint8_t*, size_t)> cb);
    bool cancel(HTTP_ID session_id);

private:
    // called from NetService
    void pause();
    void resume();
    void poll();
private:
    class Session {
    public:
        enum State { INVALID, CREATED, PROGRESS, COMPLETED, CANCELLING, CANCELLED, FAILED };
        HTTP_ID    id;
        State      state;
        bool       success;
        std::function<void(bool, const uint8_t*, size_t)> cb;
        struct {
            std::string url;
            std::unordered_multimap<std::string, std::string> headers;
            Buffer data;
        } request;
        struct {
            Buffer data;
        } response;
        Session();
        Session(Session&& o);
        ~Session();
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
        HINTERNET    hconnect, handle;
#else
    #error Not Implemented!
#endif
    };
    std::thread m_thread;
    std::mutex m_added_mutex, m_working_mutex, m_completed_mutex;
    std::list<Session> m_added, m_working, m_completed;
    UniqueId_r<HTTP_ID> m_unique;
    bool m_worker_running;

    void worker_thread();
    bool cb_session_created(Session* session);
    bool cb_session_cancelling(Session* session);
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
