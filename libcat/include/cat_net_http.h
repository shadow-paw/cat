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
#include <condition_variable>
#include <thread>
#include <atomic>
#include "cat_platform.h"
#include "cat_net_type.h"
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
                  std::function<void(const HTTP_RESPONSE&)> cb);
    bool cancel(HTTP_ID session_id);

private:
    // called from NetService
    void pause();
    void resume();
    void poll();
private:
    class HttpConnection {
    public:
        enum State { INVALID, CREATED, PROGRESS, COMPLETED, CANCELLED, FAILED };
        HTTP_ID id;
        State   state;
        std::function<void(const HTTP_RESPONSE&)> cb;
        HTTP_REQUEST  request;
        HTTP_RESPONSE response;

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
        HINTERNET    hconnect, handle;
#elif defined(PLATFORM_ANDROID)
        jobject conn, istream, ostream;
#else
    #error Not Implemented!
#endif

        HttpConnection();
        HttpConnection(HttpConnection&& o);
        ~HttpConnection();
    };
private:
    static const int POLL_INTERVAL = 100;
    std::atomic<bool> m_thread_started;
    std::thread m_thread;
    std::mutex m_added_mutex, m_working_mutex, m_completed_mutex;
    std::condition_variable m_added_condvar;
    std::list<HttpConnection> m_added, m_working, m_completed;
    UniqueId_r<HTTP_ID> m_unique;
    bool m_worker_running;

    void worker_thread();
    bool cb_conn_created(HttpConnection* conn);
    bool cb_conn_cancelled(HttpConnection* conn);
    bool cb_conn_progress(HttpConnection* conn);

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    HINTERNET  m_internet;
    struct INET_PARAM {
        HttpManager* manager;
        HTTP_ID      http_id;
    };
    static void CALLBACK cb_inet_status(HINTERNET handle, DWORD_PTR ud, DWORD status, LPVOID info, DWORD infolen);
    void cb_inet_status(HINTERNET handle, INET_PARAM* param, DWORD status, LPVOID info, DWORD infolen);
#elif defined(PLATFORM_ANDROID)
    // Nothing
#else
    #error Not Implemented!
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_NET_HTTP_H__
