#include "cat_net_http.h"
#include "cat_util_string.h"

using namespace cat;

// ----------------------------------------------------------------------------
HttpManager::HttpManager() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    m_internet = nullptr;
#else
#error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
HttpManager::~HttpManager() {
    if (m_internet) {
        InternetCloseHandle(m_internet);
        m_internet = nullptr;
    }
}
// ----------------------------------------------------------------------------
void HttpManager::pause() {
}
// ----------------------------------------------------------------------------
void HttpManager::resume() {
}
// ----------------------------------------------------------------------------
void HttpManager::poll() {
    std::forward_list<Session> list;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_sessions.empty()) return;
        list.swap(m_sessions);
    }
    for (auto it = list.begin(); it != list.end(); ++it) {
        it->cb(it->success, it->buffer, it->buflen);
    }
}
// ----------------------------------------------------------------------------
bool HttpManager::http(const char* url, const std::unordered_map<std::string, std::string>* headers, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb) {
    if (!url) {
        cb(false, nullptr, 0);
        return false;
    }
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    URL_COMPONENTS url_components = { 0 };
    Session* session = nullptr;
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

    session = new Session();
    session->manager = this;
    session->cb = cb;
    session->hconnect = InternetConnect(m_internet, server.c_str(), url_components.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!session->hconnect) goto fail;
    PCTSTR accept_types[] = { L"*/*", NULL };
    session->handle = HttpOpenRequest(session->hconnect, data?L"POST":L"GET", url_components.lpszUrlPath, NULL, NULL, accept_types,
        INTERNET_FLAG_HYPERLINK|
        INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE|
        INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP,
        (DWORD_PTR)session);
    if (headers) {
        std::string s;
        for (auto it = headers->begin(); it != headers->end(); ++it) {
            s = s + it->first + ": " + it->second + "\r\n";
        }
        std::basic_string<TCHAR> headers_t = StringUtil::string2tchar(s);
        HttpSendRequest(session->handle, headers_t.c_str(), (DWORD)headers_t.size(), const_cast<void*>(data), (DWORD)datalen);
    } else {
        HttpSendRequest(session->handle, NULL, 0, const_cast<void*>(data), (DWORD)datalen);
    }
    /*  
    InternetOpenUrl(m_internet, url_escape.c_str(), NULL, 0,
        INTERNET_FLAG_HYPERLINK |
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE |
        INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP,
        (DWORD_PTR)session);
    */
    return true;
fail:
    delete session;
    cb(false, nullptr, 0);
    return false;
#else
#error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
// Platform Specific: Windows
// ----------------------------------------------------------------------------
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
void CALLBACK HttpManager::cb_inet_status(HINTERNET handle, DWORD_PTR ud, DWORD status, LPVOID info, DWORD infolen) {
    Session* session = (Session*)ud;
    if (session)
        session->manager->cb_inet_status(session, handle, status, info, infolen);
}
void HttpManager::cb_inet_status(Session* session, HINTERNET handle, DWORD status, LPVOID info, DWORD infolen) {
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
        break;
    case INTERNET_STATUS_HANDLE_CREATED: {
        INTERNET_ASYNC_RESULT* rez = (INTERNET_ASYNC_RESULT*)info;
        session->handle = (HINTERNET)rez->dwResult;
        break;
    }
    case INTERNET_STATUS_REQUEST_COMPLETE: {
        INTERNET_ASYNC_RESULT* rez = (INTERNET_ASYNC_RESULT*)info;
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
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sessions.push_front(*session);
        }
        session->buffer = nullptr;
        delete session;
        break;
    }
    }
}
// ----------------------------------------------------------------------------
#endif