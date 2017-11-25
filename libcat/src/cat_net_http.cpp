#include "cat_net_http.h"
#include <algorithm>
#include "cat_util_string.h"
#include "cat_time_service.h"
#include "cat_util_log.h"
#if defined(PLATFORM_MAC)
  #import <Cocoa/Cocoa.h>
#elif defined(PLATFORM_IOS)
  #import <UIKit/UIKit.h>
#endif

using namespace cat;

// ----------------------------------------------------------------------------
// HttpRequest
// ----------------------------------------------------------------------------
HttpRequest::HttpRequest() {
}
HttpRequest::HttpRequest(const std::string& url) {
    m_url = url;
}
HttpRequest::HttpRequest(HttpRequest&& o) {
    m_url = std::move(o.m_url);         o.m_url.clear();
    m_headers = std::move(o.m_headers); o.m_headers.clear();
    m_data = std::move(o.m_data);       o.m_data.free();
}
HttpRequest& HttpRequest::operator=(HttpRequest&& o) {
    m_url = std::move(o.m_url);         o.m_url.clear();
    m_headers = std::move(o.m_headers); o.m_headers.clear();
    m_data = std::move(o.m_data);       o.m_data.free();
    return *this;
}
void HttpRequest::set_url(const std::string& url) {
    m_url = url;
}
void HttpRequest::add_header(const std::string& key, const std::string& value) {
    m_headers.emplace(std::make_pair(key, value));
}
void HttpRequest::post(Buffer&& data, const std::string& mime) {
    m_data = std::move(data);
    m_headers.emplace(std::make_pair("Content-Type", mime));
}
void HttpRequest::post(const std::string& data, const std::string& mime) {
    Buffer buf(data.c_str(), data.size());
    post(std::move(buf), mime);
}
// ----------------------------------------------------------------------------
// HttpResponse
// ----------------------------------------------------------------------------
HttpResponse::HttpResponse() {
    code = 0;
}
HttpResponse::HttpResponse(HttpResponse&& o) {
    code = o.code; o.code = 0;
    headers = std::move(o.headers); o.headers.clear();
    body = std::move(o.body);       o.body.free();
}
HttpResponse& HttpResponse::operator=(HttpResponse&& o) {
    code = o.code; o.code = 0;
    headers = std::move(o.headers); o.headers.clear();
    body = std::move(o.body);       o.body.free();
    return *this;
}
// ----------------------------------------------------------------------------
// HttpConnection
// ----------------------------------------------------------------------------
HttpManager::HttpConnection::HttpConnection() {
    id = 0;
    state = State::INVALID;
    response.code = 0;
    cb = nullptr;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    hconnect = nullptr;
    handle = nullptr;
#elif defined(PLATFORM_ANDROID)
    j_conn = nullptr;
    j_istream = nullptr;
    j_rbuf = nullptr;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    task = nullptr;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
HttpManager::HttpConnection::HttpConnection(HttpConnection&& o) {
    id = o.id;                       o.id = 0;
    state = o.state;                 o.state = State::INVALID;
    response.code = o.response.code; o.response.code = 0;
    cb = o.cb;                       o.cb = nullptr;
    request = std::move(o.request);
    response.body = std::move(o.response.body);

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    hconnect = o.hconnect; o.hconnect = nullptr;
    handle = o.handle;     o.handle = nullptr;
#elif defined(PLATFORM_ANDROID)
    j_conn = o.j_conn;       o.j_conn = nullptr;
    j_istream = o.j_istream; o.j_istream = nullptr;
    j_rbuf = o.j_rbuf;       o.j_rbuf = nullptr;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    task = o.task; o.task = nullptr;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
HttpManager::HttpConnection::~HttpConnection() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (handle) { InternetCloseHandle(handle); handle = nullptr; }
    if (hconnect) { InternetCloseHandle(hconnect); hconnect = nullptr; }
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    if (j_conn)    { jni.DeleteGlobalRef(j_conn);    j_conn = nullptr;    }
    if (j_istream) { jni.DeleteGlobalRef(j_istream); j_istream = nullptr; }
    if (j_rbuf)    { jni.DeleteGlobalRef(j_rbuf);    j_rbuf = nullptr;    }
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    if (task) {
        NSURLSessionTask* nstask __attribute__ ((unused)) = (__bridge_transfer NSURLSessionTask*)task;
        task = nullptr;
    }
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
// HttpManager
// ----------------------------------------------------------------------------
HttpManager::HttpManager() {
    m_unique.init(1, 0x7ffffff0, 0, HTTPID_REUSE_TIMEOUT);
    m_thread_started = false;
    m_worker_running = false;
    m_thread = nullptr;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    m_internet = nullptr;
#elif defined(PLATFORM_ANDROID)
    // Nothing
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // Nothing
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
HttpManager::~HttpManager() {
    pause();
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (m_internet) {
        InternetCloseHandle(m_internet);
        m_internet = nullptr;
    }
#elif defined(PLATFORM_ANDROID)
    // Nothing
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    // Nothing
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void HttpManager::pause() {
    if (m_thread) {
        m_worker_running = false;
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }
}
// ----------------------------------------------------------------------------
void HttpManager::resume() {
    if (m_thread_started) {
        m_worker_running = true;
        m_thread = new std::thread(&HttpManager::worker_thread, this);
    }
}
// ----------------------------------------------------------------------------
void HttpManager::poll() {
    std::unique_lock<std::mutex> lock(m_completed_mutex);
    if (m_completed.empty()) return;
    std::list<HttpConnection> list;
    list.swap(m_completed);
    lock.unlock();
    for (auto it = list.begin(); it != list.end(); ++it) {
        it->cb(it->response);
        m_unique.release(it->id, TimeService::now());
    }
}
// ----------------------------------------------------------------------------
HTTP_ID HttpManager::fetch(HttpRequest&& request, std::function<void(const HttpResponse&)> cb) {
    HTTP_ID http_id = m_unique.fetch(TimeService::now());
    HttpConnection conn;
    conn.state = HttpConnection::State::CREATED;
    conn.id = http_id;
    conn.request = std::move(request);
    conn.cb = cb;
    std::lock_guard<std::mutex> lock(m_added_mutex);
    m_added.push_back(std::move(conn));
    m_condvar.notify_all();
    // start the thread if not already
    bool expect = false;
    if (m_thread_started.compare_exchange_strong(expect, true)) {
        m_worker_running = true;
        m_thread = new std::thread(&HttpManager::worker_thread, this);
    }
    return http_id;
}
// ----------------------------------------------------------------------------
bool HttpManager::cancel(HTTP_ID http_id) {
    { // check add list
        std::lock_guard<std::mutex> lock(m_added_mutex);
        auto conn = std::find_if(m_added.begin(), m_added.end(), [&http_id](const HttpConnection& conn) {
            return conn.id == http_id;
        });
        if (conn != m_added.end()) {
            if (conn->state == HttpConnection::State::CREATED || conn->state == HttpConnection::State::PROGRESS) {
                conn->state = HttpConnection::State::CANCELLED;
            } return true;
        }
    }
    { // check working list
        std::lock_guard<std::mutex> lock(m_working_mutex);
        auto conn = std::find_if(m_working.begin(), m_working.end(), [&http_id](const HttpConnection& conn) {
            return conn.id == http_id;
        });
        if (conn != m_working.end()) {
            if (conn->state == HttpConnection::State::CREATED || conn->state == HttpConnection::State::PROGRESS) {
                conn->state = HttpConnection::State::CANCELLED;
            } return true;
        }
    } return false;
}
// ----------------------------------------------------------------------------
void HttpManager::worker_thread() {
    while (m_worker_running) {
        bool added = false;
        std::unique_lock<std::mutex> lock_added(m_added_mutex);
        std::list<HttpConnection> added_list;
        if (m_added.size() == 0) {
            m_condvar.wait_for(lock_added, std::chrono::milliseconds(POLL_INTERVAL));
        }
        if (m_added.size() > 0) {
            added_list.swap(m_added);
            added = true;
        }
        lock_added.unlock();

        std::lock_guard<std::mutex> lock_working(m_working_mutex);
        if (added) {
            m_working.splice(m_working.end(), added_list);
        }
        for (auto it=m_working.begin(); it!=m_working.end(); ) {
            bool list_changed = false;
            switch (it->state) {
            case HttpConnection::State::CREATED:
                    if (!cb_conn_created(&(*it))) {
                        it->state = HttpConnection::State::FAILED;
                        std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
                        m_completed.splice(m_completed.end(), m_working, it);
                        list_changed = true;
                    } else {
                        it->state = HttpConnection::State::PROGRESS;
                    } break;
            case HttpConnection::State::CANCELLED:
                    if (!cb_conn_cancelled(&(*it))) {
                        std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
                        m_completed.splice(m_completed.end(), m_working, it);
                        list_changed = true;
                    } break;
            case HttpConnection::State::COMPLETED:
            case HttpConnection::State::FAILED: {
                    std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
                    m_completed.splice(m_completed.end(), m_working, it);
                    list_changed = true;
                    break;
                }
            case HttpConnection::State::PROGRESS:
                    if (!cb_conn_progress(&(*it))) it->state = HttpConnection::State::FAILED;
                    if (it->state == HttpConnection::State::FAILED || it->state== HttpConnection::State::COMPLETED) {
                        std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
                        m_completed.splice(m_completed.end(), m_working, it);
                        list_changed = true;
                    } break;
            case HttpConnection::State::INVALID:
                    break;
            }
            if (list_changed) {
                it = m_working.begin();
                m_condvar.notify_all();
            } else ++it;
        }
    }
}
// ----------------------------------------------------------------------------
bool HttpManager::cb_conn_created(HttpConnection* conn) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    URL_COMPONENTS url_components = { 0 };
    TCHAR tmp[4];
    DWORD url_escape_len = 1;
    std::basic_string<TCHAR> url_escape;
    std::basic_string<TCHAR> url_t = StringUtil::make_tstring(conn->request.m_url);
    std::basic_string<TCHAR> server;
    INET_PARAM* param = nullptr;

    if (!m_internet) {
        m_internet = InternetOpen(L"Mozilla/5.0 (Windows NT x.y; Win64; x64; rv:10.0) Gecko/20100101 Firefox/10.0",
            INTERNET_OPEN_TYPE_PRECONFIG,
            NULL, NULL,
            INTERNET_FLAG_ASYNC);
        if (!m_internet) goto fail;
        InternetSetStatusCallback(m_internet, cb_inet_status);
    }

    InternetCanonicalizeUrl(url_t.c_str(), tmp, &url_escape_len, ICU_ENCODE_PERCENT);
    if (url_escape_len <= 1) return false;
    url_escape.resize(url_escape_len);
    if (!InternetCanonicalizeUrl(url_t.c_str(), &url_escape[0], &url_escape_len, ICU_ENCODE_PERCENT)) goto fail;
    url_escape.resize(url_escape_len);
    url_components.dwStructSize = sizeof(url_components);
    url_components.dwHostNameLength = 1;
    url_components.dwUrlPathLength = 1;
    InternetCrackUrl(url_escape.c_str(), 0, 0, &url_components);
    server.assign(url_components.lpszHostName, url_components.dwHostNameLength);

    param = new INET_PARAM();
    param->manager = this;
    param->http_id = conn->id;
    conn->hconnect = InternetConnect(m_internet, server.c_str(), url_components.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!conn->hconnect) goto fail;
    PCTSTR accept_types[] = { L"*/*", NULL };
    DWORD flags = INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE;
    if (url_components.nScheme == INTERNET_SCHEME_HTTPS) flags |= INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP;
    conn->handle = HttpOpenRequest(conn->hconnect,
                                   conn->request.m_data ? L"POST" : L"GET",
                                   url_components.lpszUrlPath, NULL, NULL,
                                   accept_types,
                                   flags, (DWORD_PTR)param);
    if (conn->request.m_headers.size()>0) {
        std::string s;
        for (auto it=conn->request.m_headers.begin(); it!=conn->request.m_headers.end(); ++it) {
            s = s + it->first + ": " + it->second + "\r\n";
        }
        std::basic_string<TCHAR> headers_t = StringUtil::make_tstring(s);
        HttpSendRequest(conn->handle, headers_t.c_str(), (DWORD)headers_t.size(), const_cast<uint8_t*>(conn->request.m_data.ptr()), (DWORD)conn->request.m_data.size());
    } else {
        HttpSendRequest(conn->handle, NULL, 0, const_cast<uint8_t*>(conn->request.m_data.ptr()), (DWORD)conn->request.m_data.size());
    }
    return true;
fail:
    if (param) delete param;
    return false;
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    // URL j_url = new URL(conn->request.url);
    jobject j_url = jni.NewObject("java/net/URL", "(Ljava/lang/String;)V", jni.NewStringUTF(conn->request.m_url));
    if (!j_url) return false;
    // HttpURLConnection j_conn = j_url.openConnection();
    jobject j_conn = jni.CallObjectMethod(j_url, "openConnection", "()Ljava/net/URLConnection;");
    if (!j_conn) return false;
    conn->j_conn = jni.NewGlobalRef(j_conn);
    // Request Header
    for (auto it = conn->request.m_headers.begin(); it != conn->request.m_headers.end(); ++it) {
        jni.CallVoidMethod(j_conn, "addRequestProperty", "(Ljava/lang/String;Ljava/lang/String;)V", jni.NewStringUTF(it->first), jni.NewStringUTF(it->second));
    }
    // Post
    if (conn->request.m_data.ptr() && conn->request.m_data.size() > 0) {
        Logger::d("libcat", "http post");
        // j_conn.setDoOutput(true);
        jni.CallVoidMethod(j_conn, "setDoOutput", "(Z)V", JNI_TRUE);
        // j_conn.setFixedLengthStreamingMode(postlen);
        jni.CallVoidMethod(j_conn, "setFixedLengthStreamingMode", "(I)V", (jint)conn->request.m_data.size());
        // OutputStream j_os = j_conn.getOutputStream();
        jobject j_os = jni.CallObjectMethod(j_conn, "getOutputStream", "()Ljava/io/OutputStream;");
        // j_os.write(conn->request.data); 
        jni.CallVoidMethod(j_os, "write", "([B)V", jni.NewByteArray(conn->request.m_data.ptr(), conn->request.m_data.size()));
    }
    // InputStream j_in = new BufferedInputStream(j_conn.getInputStream());
    jobject j_istream = jni.NewObject("java/io/BufferedInputStream", "(Ljava/io/InputStream;)V", jni.CallObjectMethod(j_conn, "getInputStream", "()Ljava/io/InputStream;"));
    conn->j_istream = jni.NewGlobalRef(j_istream);
    conn->j_rbuf = (jbyteArray)jni.NewGlobalRef(jni.NewByteArray(RBUF_SIZE));
    return true;
fail:
    return false;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    HTTP_ID http_id = conn->id;
    // completion handler
    void (^cb_complete)(NSData *data, NSURLResponse *response, NSError *error) = ^(NSData *data, NSURLResponse *response, NSError *error) {
        std::lock_guard<std::mutex> lock(m_working_mutex);
        auto conn = std::find_if(m_working.begin(), m_working.end(), [&http_id](const HttpConnection& conn) {
            return conn.id == http_id;
        });
        if (conn == m_working.end()) return;
    
        NSHTTPURLResponse* hres = (NSHTTPURLResponse*)response;
        conn->response.code = (int)hres.statusCode;
        for (NSString* key in hres.allHeaderFields) {
            NSString* value = hres.allHeaderFields[key];
            conn->response.headers.emplace(std::make_pair(key.UTF8String, value.UTF8String));
        }
        conn->response.body.realloc((size_t)data.length+1);
        conn->response.body.copy(0, data.bytes, (size_t)data.length);
        conn->response.body[data.length] = 0;      // put a zero after payload for convenience
        conn->response.body.shrink(data.length);   // length not include that zero
        conn->state = HttpConnection::State::COMPLETED;
        
        std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
        m_completed.splice(m_completed.end(), m_working, conn);
    };
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration defaultSessionConfiguration];
    // Request Header
    if (conn->request.m_headers.size() > 0) {
        NSMutableDictionary* headers = [[NSMutableDictionary alloc] init];
        for (auto it = conn->request.m_headers.begin(); it != conn->request.m_headers.end(); ++it) {
            [headers setValue:[NSString stringWithUTF8String:it->second.c_str()] forKey:[NSString stringWithUTF8String:it->first.c_str()]];
        }
        config.HTTPAdditionalHeaders = headers;
    }
    NSURLSession *session = [NSURLSession sessionWithConfiguration:config];
    NSURL* url = [NSURL URLWithString:[NSString stringWithUTF8String:conn->request.m_url.c_str()]];
    // Post
    if (conn->request.m_data.ptr() && conn->request.m_data.size() > 0) {
        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
        request.HTTPMethod = @"POST";
        NSURLSessionUploadTask *uploadTask = [session uploadTaskWithRequest:request fromData:[NSData dataWithBytes:conn->request.m_data.ptr() length:conn->request.m_data.size()] completionHandler:cb_complete];
        [uploadTask resume];
        conn->task = (__bridge_retained void*)uploadTask;
    } else {
        NSURLSessionDataTask *dataTask = [session dataTaskWithURL:url completionHandler:cb_complete];
        [dataTask resume];
        conn->task = (__bridge_retained void*)dataTask;
    } return true;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool HttpManager::cb_conn_cancelled(HttpConnection* conn) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (conn->hconnect) { InternetCloseHandle(conn->hconnect); conn->hconnect = nullptr; }
    if (conn->handle) {   
        InternetCloseHandle(conn->handle);
        conn->handle = nullptr;
        return false;   // do not remove entry from list, let the callback do cleanup
    } return true;
#elif defined(PLATFORM_ANDROID)
    return true;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    NSURLSessionTask* nstask = (__bridge NSURLSessionTask*)conn->task;
    [nstask cancel];
    return true;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool HttpManager::cb_conn_progress(HttpConnection* conn) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    return true;
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    int len = jni.CallIntMethod(conn->j_istream, "read", "([B)I", conn->j_rbuf);
    if (len == -1) {    // done
        conn->response.code = jni.CallIntMethod(conn->j_conn, "getResponseCode", "()I");
        for (int i=0; i<1024; i++) {
            jstring j_key   = (jstring)jni.CallObjectMethod(conn->j_conn, "getHeaderFieldKey", "(I)Ljava/lang/String;", i);
            jstring j_value = (jstring)jni.CallObjectMethod(conn->j_conn, "getHeaderField", "(I)Ljava/lang/String;", i);
            if (!j_key || !j_value) break;
            conn->response.headers.emplace(std::make_pair(jni.GetStringUTFChars(j_key), jni.GetStringUTFChars(j_value)));
        }
        conn->state = HttpConnection::State::COMPLETED;
    } else if (len > 0) {
        size_t cursor = conn->response.body.size();
        if (!conn->response.body.realloc(cursor + len)) return false;
        jni.GetByteArrayRegion(conn->j_rbuf, conn->response.body.ptr() + cursor, 0, (size_t)len);
        conn->response.body[cursor + len] = 0;      // put a zero after payload for convenience
        conn->response.body.shrink(cursor + len);   // length not include that zero
    }
    return true;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    return true;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
// Platform Specific: Windows
// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
void CALLBACK HttpManager::cb_inet_status(HINTERNET handle, DWORD_PTR ud, DWORD status, LPVOID info, DWORD infolen) {
    INET_PARAM* param = reinterpret_cast<INET_PARAM*>(ud);
    if (param) param->manager->cb_inet_status(handle, param, status, info, infolen);
}
void HttpManager::cb_inet_status(HINTERNET handle, INET_PARAM* param, DWORD status, LPVOID info, DWORD infolen) {
    switch (status) {
    case INTERNET_STATUS_DETECTING_PROXY:
    case INTERNET_STATUS_RESOLVING_NAME:
    case INTERNET_STATUS_NAME_RESOLVED:
    case INTERNET_STATUS_CONNECTING_TO_SERVER:
    case INTERNET_STATUS_CONNECTED_TO_SERVER:
    case INTERNET_STATUS_SENDING_REQUEST:
    case INTERNET_STATUS_REQUEST_SENT:
    case INTERNET_STATUS_RECEIVING_RESPONSE:
    case INTERNET_STATUS_RESPONSE_RECEIVED:
    case INTERNET_STATUS_REDIRECT:
    case INTERNET_STATUS_COOKIE_SENT:
    case INTERNET_STATUS_P3P_HEADER:
    case INTERNET_STATUS_COOKIE_RECEIVED:
    case INTERNET_STATUS_HANDLE_CREATED:
            break;
    case INTERNET_STATUS_REQUEST_COMPLETE: {
            INTERNET_ASYNC_RESULT* rez = (INTERNET_ASYNC_RESULT*)info;
            HTTP_ID http_id = param->http_id;
            std::lock_guard<std::mutex> lock(m_working_mutex);
            auto conn = std::find_if(m_working.begin(), m_working.end(), [&http_id](const HttpConnection& conn) {
                return conn.id == http_id;
            });
            if (conn == m_working.end()) break;

            bool success = rez->dwResult != 0;
            if (success) {
                DWORD bodylen = 0;
                if (!InternetQueryDataAvailable(conn->handle, &bodylen, 0, 0)) {
                    success = false;
                } else {
                    conn->response.body.realloc(bodylen + 1);
                    DWORD rlen = 0;
                    if (!InternetReadFile(conn->handle, conn->response.body, bodylen, &rlen)) {
                        success = false;
                    } else {
                        if (rlen <= bodylen+1) conn->response.body[rlen] = 0;
                        conn->response.body.shrink((size_t)rlen);
                    }
                }
                {   // response code
                    DWORD code = 0;
                    DWORD codesize = (DWORD)sizeof(code);
                    HttpQueryInfo(conn->handle, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &code, &codesize, nullptr);
                    conn->response.code = (int)code;
                    // response headers
                    DWORD headersize = 0;
                    HttpQueryInfo(conn->handle, HTTP_QUERY_RAW_HEADERS, nullptr, &headersize, nullptr);
                    if (headersize > 0) {
                        std::unique_ptr<TCHAR[]> header_tstrings(new TCHAR[headersize]);
                        HttpQueryInfo(conn->handle, HTTP_QUERY_RAW_HEADERS, header_tstrings.get(), &headersize, nullptr);
                        StringUtil::tstrings_each(header_tstrings.get(), [&conn](const std::string& header) -> bool {
                            auto pos = header.find(":");
                            if (pos != std::string::npos) {
                                std::string key(header, 0, pos);
                                std::string value(header, pos+1);
                                conn->response.headers.insert(std::pair<std::string,std::string>(StringUtil::trim(key), StringUtil::trim(value)));
                            } return true;
                        });
                    }
                }
            }
            InternetSetStatusCallback(conn->handle, NULL);
            InternetCloseHandle(conn->handle);   conn->handle = nullptr;
            InternetCloseHandle(conn->hconnect); conn->hconnect = nullptr;
            if (conn->state!=HttpConnection::State::CANCELLED) conn->state = HttpConnection::State::COMPLETED;
            std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
            m_completed.splice(m_completed.end(), m_working, conn);
            delete param;
            m_condvar.notify_all();
            break;
        }
    }
}
// ----------------------------------------------------------------------------
#endif
