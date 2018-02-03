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
#include "nlohmann/json.hpp"
#include "cat_platform.h"
#include "cat_data_copyable.h"
#include "cat_data_buffer.h"
#include "cat_data_uniqueid.h"
#include "cat_time_type.h"

namespace cat {
class NetService;
class HttpManager;
// ----------------------------------------------------------------------------
typedef unsigned int HTTP_ID;
// ----------------------------------------------------------------------------
//! HTTP Request
class HttpRequest {
friend class HttpManager;
public:
    enum Method {
        METHOD_GET, METHOD_POST, METHOD_PUT, METHOD_PATCH, METHOD_DELETE
    };
    //! Constructor an empty request, you must call set_url()
    //! \sa set_url
    HttpRequest();
    //! Constructor with url
    //! \param url Url of the request, e.g. "https://google.com/"
    HttpRequest(const std::string& url, Method method = Method::METHOD_GET);
    //! Move Constructor
    HttpRequest(HttpRequest&& o);
    //! Move Assignment
    HttpRequest& operator=(HttpRequest&& o);

    //! Set the url of the request
    //! \param url Url of the request, e.g. "https://google.com/"
    void set_url(const std::string& url);
    //! Add a custom request header, a request can have multiple header of same key
    //! \param key Key of header
    //! \param value Value of header
    void add_header(const std::string& key, const std::string& value);
    //! Make this a post request with the provide data
    //! \param data Data to post, the data will be acquired by HTTP Manager with move semantic
    //! \param mime MIME type, e.g. "application/json"
    void post(Buffer&& data, const std::string& mime);
    //! Make this a post request with the provide data
    //! \param data String to post, zero-terminator is excluded
    //! \param mime MIME type, e.g. "application/json"
    void post(const std::string& data, const std::string& mime);
    //! Make this a post request with the provide data
    //! \param json json data to post
    void post(const nlohmann::json& json);

    //! Make this a put request with the provide data
    //! \param data Data to post, the data will be acquired by HTTP Manager with move semantic
    //! \param mime MIME type, e.g. "application/json"
    void put(Buffer&& data, const std::string& mime);
    //! Make this a put request with the provide data
    //! \param data String to post, zero-terminator is excluded
    //! \param mime MIME type, e.g. "application/json"
    void put(const std::string& data, const std::string& mime);
    //! Make this a put request with the provide data
    //! \param json json data to post
    void put(const nlohmann::json& json);

    //! Make this a patch request with the provide data
    //! \param data Data to post, the data will be acquired by HTTP Manager with move semantic
    //! \param mime MIME type, e.g. "application/json"
    void patch(Buffer&& data, const std::string& mime);
    //! Make this a patch request with the provide data
    //! \param data String to post, zero-terminator is excluded
    //! \param mime MIME type, e.g. "application/json"
    void patch(const std::string& data, const std::string& mime);
    //! Make this a patch request with the provide data
    //! \param json json data to post
    void patch(const nlohmann::json& json);

    //! Make this a delete request
    void del();
private:
    Method m_method;
    std::string m_url;
    std::unordered_multimap<std::string, std::string> m_headers;
    Buffer m_data;
    const char* method_string() const;
};
// ----------------------------------------------------------------------------
//! HTTP Response
class HttpResponse {
public:
    //! HTTP status code, e.g. 200
    int code;
    //! Response header
    std::unordered_multimap<std::string, std::string> headers;
    //! Response body
    Buffer body;

    //! Construct empty response
    HttpResponse();
    //! Move Constructor
    HttpResponse(HttpResponse&& o);
    //! Move Assignment
    HttpResponse& operator=(HttpResponse&& o);
};
// ----------------------------------------------------------------------------
//! HTTP Manager
class HttpManager : private NonCopyable<HttpManager> {
friend class cat::NetService;
public:
    HttpManager();
    ~HttpManager();

    //! Start an HTTP session
    //! \param request HTTP request
    //! \param cb Callback upon complete/failure, will be invoked from main thread
    //! \return HTTP id used for cancallation
    //! \sa HttpRequest, cancel
    HTTP_ID fetch(HttpRequest&& request, std::function<void(HttpResponse&&)> cb);
    //! Cancel an HTTP session
    //! \param http_id HTTP id obtained from fetch
    //! \return true if cancelled, false if cannot be cancelled
    //! \sa fetch
    bool cancel(HTTP_ID http_id);

private:
    //! Called from NetService when the app is put to background
    void pause();
    //! Called from NetService when the app is resume to foreground
    void resume();
    //! Called from NetService to poll for event
    void poll();
private:
    class HttpConnection {
    public:
        enum State { INVALID, CREATED, PROGRESS, COMPLETED, CANCELLED, FAILED };
        HTTP_ID id;
        State   state;
        std::function<void(HttpResponse&&)> cb;
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
    const Timestamp HTTPID_REUSE_TIMEOUT = 5000;
    const int POLL_INTERVAL = 100;
    std::atomic<bool> m_thread_started;
    std::thread* m_thread;
    std::mutex m_added_mutex, m_working_mutex, m_completed_mutex;
    std::condition_variable m_condvar;
    std::list<HttpConnection> m_added, m_working, m_completed;
    UniqueId<HTTP_ID,Timestamp> m_unique;
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
