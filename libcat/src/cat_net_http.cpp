#include "cat_net_http.h"
#include <algorithm>
#include <chrono>
#include "cat_util_string.h"
#include "cat_time_service.h"
#include "cat_util_log.h"

using namespace cat;

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
#else
#error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
HttpManager::HttpConnection::HttpConnection(HttpConnection&& o) {
    id = o.id;
    state = o.state;
    response.code = o.response.code;
    cb = o.cb;
    o.id = 0;
    o.state = State::INVALID;
    o.response.code = 0;
    o.cb = nullptr;

    request.url = std::move(o.request.url);
    request.headers = std::move(o.request.headers);
    request.data = std::move(o.request.data);
    response.body = std::move(o.response.body);

#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    hconnect = o.hconnect;
    handle = o.handle;
    o.hconnect = nullptr;
    o.handle = nullptr;
#else
#error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
HttpManager::HttpConnection::~HttpConnection() {
}
// ----------------------------------------------------------------------------
// HttpManager
// ----------------------------------------------------------------------------
HttpManager::HttpManager() {
    m_unique.init(5000, 1, 0x7ffffff0);
    m_thread_started = false;
    m_worker_running = false;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    m_internet = nullptr;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
HttpManager::~HttpManager() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (m_internet) {
        InternetCloseHandle(m_internet);
        m_internet = nullptr;
    }
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void HttpManager::pause() {
    if (m_thread_started) {
        m_worker_running = false;
        m_thread.join();
    }
}
// ----------------------------------------------------------------------------
void HttpManager::resume() {
    if (m_thread_started) {
        m_worker_running = true;
        m_thread = std::thread(&HttpManager::worker_thread, this);
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
        m_unique.release(TimeService::now(), it->id);
    }
}
// ----------------------------------------------------------------------------
HTTP_ID HttpManager::fetch(const std::string& url,
                           std::unordered_multimap<std::string, std::string>&& headers,
                           Buffer&& data,
                           std::function<void(const HTTP_RESPONSE&)> cb) {
    HTTP_ID http_id = m_unique.fetch(TimeService::now());
    HttpConnection conn;
    conn.state = HttpConnection::State::CREATED;
    conn.id = http_id;
    conn.request.url = url;
    conn.request.headers = std::move(headers);
    conn.request.data = std::move(data);
    conn.cb = cb;
    std::lock_guard<std::mutex> lock(m_added_mutex);
    m_added.push_back(std::move(conn));
    m_added_condvar.notify_all();
    // start the thread if not already
    bool expect = false;
    if (m_thread_started.compare_exchange_strong(expect, true)) {
        m_worker_running = true;
        m_thread = std::thread(&HttpManager::worker_thread, this);
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
        std::unique_lock<std::mutex> lock_added(m_added_mutex);
        if (m_added.size() == 0) {
            m_added_condvar.wait_for(lock_added, std::chrono::milliseconds(POLL_INTERVAL));
            if (m_added.size() == 0) continue;
        }
        std::list<HttpConnection> added_list;
        added_list.swap(m_added);
        lock_added.unlock();

        std::lock_guard<std::mutex> lock_working(m_working_mutex);
        m_working.splice(m_working.end(), added_list);

        for (auto it=m_working.begin(); it!=m_working.end(); ) {
            bool list_changed = false;
            switch (it->state) {
            case HttpConnection::State::CREATED:
                    cb_conn_created(&(*it));
                    break;
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
                    cb_conn_progress(&(*it));
                    break;
            }
            if (list_changed) it = m_working.begin();
            else ++it;
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
    std::basic_string<TCHAR> url_t = StringUtil::make_tstring(conn->request.url);
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
                                   conn->request.data ? L"POST" : L"GET",
                                   url_components.lpszUrlPath, NULL, NULL,
                                   accept_types,
                                   flags, (DWORD_PTR)param);
    if (conn->request.headers.size()>0) {
        std::string s;
        for (auto it=conn->request.headers.begin(); it!=conn->request.headers.end(); ++it) {
            s = s + it->first + ": " + it->second + "\r\n";
        }
        std::basic_string<TCHAR> headers_t = StringUtil::make_tstring(s);
        HttpSendRequest(conn->handle, headers_t.c_str(), (DWORD)headers_t.size(), const_cast<uint8_t*>(conn->request.data.ptr()), (DWORD)conn->request.data.size());
    } else {
        HttpSendRequest(conn->handle, NULL, 0, const_cast<uint8_t*>(conn->request.data.ptr()), (DWORD)conn->request.data.size());
    }
    conn->state = HttpConnection::State::PROGRESS;
    return true;
fail:
    if (param) delete param;
    conn->state = HttpConnection::State::FAILED;
    return false;
#else
#error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool HttpManager::cb_conn_cancelled(HttpConnection* conn) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    conn->state = HttpConnection::State::CANCELLED;
    if (conn->hconnect) { InternetCloseHandle(conn->hconnect); conn->hconnect = nullptr; }
    if (conn->handle) {
        InternetCloseHandle(conn->handle);
        conn->handle = nullptr;
        return true;
    } return false;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool HttpManager::cb_conn_progress(HttpConnection* conn) {
    return true;
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
                    conn->response.body.alloc(bodylen + 1);
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
                        std::unique_ptr<TCHAR> header_tstrings(new TCHAR[headersize]);
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
            InternetCloseHandle(conn->handle);
            InternetCloseHandle(conn->hconnect);
            conn->handle = nullptr;
            conn->hconnect = nullptr;
            if (conn->state!=HttpConnection::State::CANCELLED) conn->state = HttpConnection::State::COMPLETED;
            std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
            m_completed.splice(m_completed.end(), m_working, conn);
            delete param;
            break;
        }
    }
}
// ----------------------------------------------------------------------------
#endif