#include "cat_net_http.h"
#include <algorithm>
#include "cat_util_string.h"
#include "cat_time_service.h"

using namespace cat;

// ----------------------------------------------------------------------------
HttpManager::Session::Session() {
    id = 0;
    cancelled = false;
    success = false;
    buffer = nullptr;
    buflen = 0;
    cb = nullptr;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    hconnect = nullptr;
    handle = nullptr;
#else
    #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
HttpManager::Session::Session(Session&& o) {
    id = o.id;
    cancelled = o.cancelled;
    success = o.success;
    buffer = o.buffer;
    buflen = o.buflen;
    cb = o.cb;
    o.id = 0;
    o.cancelled = false;
    o.success = false;
    o.buffer = nullptr;
    o.buflen = 0;
    o.cb = nullptr;
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
HttpManager::Session::~Session() {
    if (buffer) { delete buffer; buffer = nullptr; }
}
// ----------------------------------------------------------------------------
HttpManager::HttpManager() {
    m_unique.init(5000, 1, 0x7ffffff0);
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
}
// ----------------------------------------------------------------------------
void HttpManager::resume() {
}
// ----------------------------------------------------------------------------
void HttpManager::poll() {
    std::list<Session> list;
    {
        std::lock_guard<std::mutex> lock(m_complete_mutex);
        if (m_complete.empty()) return;
        list.swap(m_complete);
    }
    for (auto it = list.begin(); it != list.end(); ++it) {
        it->cb(it->success, it->buffer, it->buflen);
        m_unique.release(TimeService::now(), it->id);
    }
}
// ----------------------------------------------------------------------------
bool HttpManager::cancel(HTTP_ID http_id){
    std::lock_guard<std::mutex> lock(m_progress_mutex);
    auto session = std::find_if(m_progress.begin(), m_progress.end(), [&http_id](const Session& session) {
        return session.id == http_id;
    });
    if (session == m_progress.end()) return false;
    if (session->cancelled || session->success) return false;
    session->cancelled = true;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    InternetCloseHandle(session->handle);
#else
    #error Not Implemented!
#endif
    return true;
}
// ----------------------------------------------------------------------------
HTTP_ID HttpManager::fetch(const char* url, const std::unordered_map<std::string, std::string>* headers, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb) {
    if (!url) {
        cb(false, nullptr, 0);
        return false;
    }
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    URL_COMPONENTS url_components = { 0 };
    INET_PARAM* param = nullptr;
    HTTP_ID http_id;
    Session session;
    std::lock_guard<std::mutex> lock(m_progress_mutex);

    if (!m_internet) {
        m_internet = InternetOpen(L"Mozilla/5.0 (Windows NT x.y; Win64; x64; rv:10.0) Gecko/20100101 Firefox/10.0",
            INTERNET_OPEN_TYPE_PRECONFIG,
            NULL, NULL,
            INTERNET_FLAG_ASYNC);
        if (!m_internet) return false;
        InternetSetStatusCallback(m_internet, cb_inet_status);
    }
    // Create Handle
    TCHAR tmp[4];
    DWORD url_escape_len = 1;
    std::basic_string<TCHAR> url_escape;
    std::basic_string<TCHAR> url_t = StringUtil::string2tchar(url);
    std::basic_string<TCHAR> server;
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

    http_id = m_unique.fetch(TimeService::now());
    param = new INET_PARAM();
    param->manager = this;
    param->http_id = http_id;
    session.id = http_id;
    session.cb = cb;
    session.hconnect = InternetConnect(m_internet, server.c_str(), url_components.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!session.hconnect) goto fail;
    PCTSTR accept_types[] = { L"*/*", NULL };
    session.handle = HttpOpenRequest(session.hconnect, data?L"POST":L"GET", url_components.lpszUrlPath, NULL, NULL, accept_types,
        INTERNET_FLAG_HYPERLINK|
        INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE|
        INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP,
        (DWORD_PTR)param);
    if (headers) {
        std::string s;
        for (auto it = headers->begin(); it != headers->end(); ++it) {
            s = s + it->first + ": " + it->second + "\r\n";
        }
        std::basic_string<TCHAR> headers_t = StringUtil::string2tchar(s);
        HttpSendRequest(session.handle, headers_t.c_str(), (DWORD)headers_t.size(), const_cast<void*>(data), (DWORD)datalen);
    } else {
        HttpSendRequest(session.handle, NULL, 0, const_cast<void*>(data), (DWORD)datalen);
    }
    m_progress.push_back(std::move(session));
    return http_id;
fail:
    if (param) delete param;
    cb(false, nullptr, 0);
    return 0;
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
    if (param) param->manager->cb_inet_status(param, handle, status, info, infolen);
}
void HttpManager::cb_inet_status(INET_PARAM* param, HINTERNET handle, DWORD status, LPVOID info, DWORD infolen) {
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
            std::lock_guard<std::mutex> lock(m_progress_mutex);
            auto session = std::find_if(m_progress.begin(), m_progress.end(), [&http_id](const Session& session) {
                return session.id == http_id;
            });
            if (session == m_progress.end()) break;

            session->success = rez->dwResult != 0;
            if (session->success) {
                DWORD bodylen = 0;
                if (!InternetQueryDataAvailable(session->handle, &bodylen, 0, 0)) {
                    session->success = false;
                } else {
                    session->buffer = new uint8_t[bodylen + 1];
                    DWORD rlen = 0;
                    if (!InternetReadFile(session->handle, session->buffer, bodylen, &rlen)) {
                        session->success = false;
                    } else {
                        session->buflen = (size_t)rlen;
                        session->buffer[session->buflen] = 0;
                    }
                }
            }
            InternetCloseHandle(session->handle);
            InternetCloseHandle(session->hconnect);
            session->handle = nullptr;
            session->hconnect = nullptr;
            std::lock_guard<std::mutex> lock_complete(m_complete_mutex);
            m_complete.splice(m_complete.end(), m_progress, session);
            delete param;
            break;
        }
    }
}
// ----------------------------------------------------------------------------
#endif