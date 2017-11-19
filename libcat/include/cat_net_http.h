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
#include "cat_data_buffer.h"
#include "cat_data_uniqueid.h"

namespace cat {
class NetService;
class HttpManager;
// ----------------------------------------------------------------------------
typedef unsigned int HTTP_ID;
// ----------------------------------------------------------------------------
class HttpRequest {
friend class HttpManager;
public:
    HttpRequest();
    HttpRequest(const std::string& url);
    HttpRequest(HttpRequest&& o);
    HttpRequest& operator=(HttpRequest&& o);

    void set_url(const std::string& url);
    void add_header(const std::string& key, const std::string& value);
    void post(Buffer&& data, const std::string& mime);
    void post(const std::string& data, const std::string& mime);
private:
    std::string m_url;
    std::unordered_multimap<std::string, std::string> m_headers;
    Buffer m_data;
};
// ----------------------------------------------------------------------------
class HttpResponse {
public:
    int code;
    std::unordered_multimap<std::string, std::string> headers;
    Buffer body;

    HttpResponse();
    HttpResponse(HttpResponse&& o);
    HttpResponse& operator=(HttpResponse&& o);
};
// ----------------------------------------------------------------------------
class HttpManager {
friend class cat::NetService;
public:
    HttpManager();
    ~HttpManager();

    HTTP_ID fetch(HttpRequest&& request, std::function<void(const HttpResponse&)> cb);
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
        std::function<void(const HttpResponse&)> cb;
        HttpRequest  request;
        HttpResponse response;

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
        HINTERNET    hconnect, handle;
#elif defined(PLATFORM_ANDROID)
        jobject j_conn, j_istream;
        jbyteArray j_rbuf;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
        void* task;
#else
    #error Not Implemented!
#endif

        HttpConnection();
        HttpConnection(HttpConnection&& o);
        ~HttpConnection();
    };
private:
    const int POLL_INTERVAL = 100;
    std::atomic<bool> m_thread_started;
    std::thread* m_thread;
    std::mutex m_added_mutex, m_working_mutex, m_completed_mutex;
    std::condition_variable m_condvar;
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
    static const size_t RBUF_SIZE = 4096;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // Nothing
#else
    #error Not Implemented!
#endif
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_NET_HTTP_H__
