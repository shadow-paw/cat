#include "cat_net_http.h"
#include <algorithm>
#include <chrono>
#include "cat_util_string.h"
#include "cat_time_service.h"
#include "cat_util_log.h"

using namespace cat;

// ----------------------------------------------------------------------------
HttpManager::Session::Session() {
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
HttpManager::Session::Session(Session&& o) {
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
    response.data = std::move(o.response.data);

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
    m_worker_running = false;
    m_thread.join();
}
// ----------------------------------------------------------------------------
void HttpManager::resume() {
    m_worker_running = true;
    m_thread = std::thread(&HttpManager::worker_thread, this);
}
// ----------------------------------------------------------------------------
void HttpManager::poll() {
    std::list<Session> list;
    {
        std::lock_guard<std::mutex> lock(m_completed_mutex);
        if (m_completed.empty()) return;
        list.splice(list.end(), m_completed);
    }
    for (auto it = list.begin(); it != list.end(); ++it) {
        it->cb(it->response.code, it->response.data.ptr(), it->response.data.size());
        m_unique.release(TimeService::now(), it->id);
    }
}
// ----------------------------------------------------------------------------
bool HttpManager::cancel(HTTP_ID http_id) {
    { // check add list
        std::lock_guard<std::mutex> lock(m_added_mutex);
        auto session = std::find_if(m_added.begin(), m_added.end(), [&http_id](const Session& session) {
            return session.id == http_id;
        });
        if (session != m_added.end()) {
            if (session->state == Session::State::CREATED || session->state == Session::State::PROGRESS) {
                session->state = Session::State::CANCELLING;
            } return true;
        }
    }
    { // check working list
        std::lock_guard<std::mutex> lock(m_working_mutex);
        auto session = std::find_if(m_working.begin(), m_working.end(), [&http_id](const Session& session) {
            return session.id == http_id;
        });
        if (session != m_working.end()) {
            if (session->state == Session::State::CREATED || session->state == Session::State::PROGRESS) {
                session->state = Session::State::CANCELLING;
            } return true;
        }
    } return false;
}
// ----------------------------------------------------------------------------
HTTP_ID HttpManager::fetch(const std::string& url,
                           std::unordered_multimap<std::string, std::string>&& headers,
                           Buffer&& data,
                           std::function<void(int, const uint8_t*, size_t)> cb) {
    Session session;
    HTTP_ID http_id = m_unique.fetch(TimeService::now());
    session.state = Session::State::CREATED;
    session.id = http_id;
    session.request.url = url;
    session.request.headers = std::move(headers);
    session.request.data = std::move(data);
    session.cb = cb;
    std::lock_guard<std::mutex> lock(m_added_mutex);
    m_added.push_back(std::move(session));
    return http_id;
}
// ----------------------------------------------------------------------------
void HttpManager::worker_thread() {
    while (m_worker_running) {
        m_working_mutex.lock();
        {
            std::lock_guard<std::mutex> lock(m_added_mutex);
            m_working.splice(m_working.end(), m_added);
        }
        for (auto it=m_working.begin(); it!=m_working.end(); ) {
            switch (it->state) {
            case Session::State::CREATED:
                cb_session_created(&(*it));
                ++it;
                break;
            case Session::State::CANCELLING:
                if (!cb_session_cancelling(&(*it))) {
                    std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
                    m_completed.splice(m_completed.end(), m_working, it);
                    it = m_working.begin();
                } else ++it;
                break;
            default:
                ++it;
            }
        }
        m_working_mutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
// ----------------------------------------------------------------------------
bool HttpManager::cb_session_created(Session* session) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    URL_COMPONENTS url_components = { 0 };
    TCHAR tmp[4];
    DWORD url_escape_len = 1;
    std::basic_string<TCHAR> url_escape;
    std::basic_string<TCHAR> url_t = StringUtil::string2tchar(session->request.url);
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
    param->http_id = session->id;
    session->hconnect = InternetConnect(m_internet, server.c_str(), url_components.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!session->hconnect) goto fail;
    PCTSTR accept_types[] = { L"*/*", NULL };
    session->handle = HttpOpenRequest(session->hconnect, session->request.data ? L"POST" : L"GET", url_components.lpszUrlPath, NULL, NULL, accept_types,
        INTERNET_FLAG_HYPERLINK |
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE |
        INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP,
        (DWORD_PTR)param);
    if (session->request.headers.size()>0) {
        std::string s;
        for (auto it = session->request.headers.begin(); it != session->request.headers.end(); ++it) {
            s = s + it->first + ": " + it->second + "\r\n";
        }
        std::basic_string<TCHAR> headers_t = StringUtil::string2tchar(s);
        HttpSendRequest(session->handle, headers_t.c_str(), (DWORD)headers_t.size(), const_cast<uint8_t*>(session->request.data.ptr()), (DWORD)session->request.data.size());
    } else {
        HttpSendRequest(session->handle, NULL, 0, const_cast<uint8_t*>(session->request.data.ptr()), (DWORD)session->request.data.size());
    }
    session->state = Session::State::PROGRESS;
    return true;
fail:
    if (param) delete param;
    session->state = Session::State::FAILED;
    session->cb(false, nullptr, 0);
    return false;
#else
#error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
bool HttpManager::cb_session_cancelling(Session* session) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    session->state = Session::State::CANCELLED;
    if (session->hconnect) { InternetCloseHandle(session->hconnect); session->hconnect = nullptr; }
    if (session->handle) {
        InternetCloseHandle(session->handle); 
        session->handle = nullptr;
        return true;
    } return false;
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
            std::lock_guard<std::mutex> lock(m_working_mutex);
            auto session = std::find_if(m_working.begin(), m_working.end(), [&http_id](const Session& session) {
                return session.id == http_id;
            });
            if (session == m_working.end()) break;

            bool success = rez->dwResult != 0;
            if (success) {
                DWORD bodylen = 0;
                if (!InternetQueryDataAvailable(session->handle, &bodylen, 0, 0)) {
                    success = false;
                } else {
                    session->response.data.alloc(bodylen + 1);
                    DWORD rlen = 0;
                    if (!InternetReadFile(session->handle, session->response.data, bodylen, &rlen)) {
                        success = false;
                    } else {
                        if (rlen <= bodylen+1) session->response.data[rlen] = 0;
                        session->response.data.shrink((size_t)rlen);
                    }
                }
                // response code
                {
                    DWORD code = 0;
                    DWORD codesize = (DWORD)sizeof(code);
                    HttpQueryInfo(session->handle, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &code, &codesize, 0);
                    session->response.code = (int)code;
                }
            }
            InternetCloseHandle(session->handle);
            InternetCloseHandle(session->hconnect);
            session->handle = nullptr;
            session->hconnect = nullptr;
            session->state = Session::State::COMPLETED;
            std::lock_guard<std::mutex> lock_complete(m_completed_mutex);
            m_completed.splice(m_completed.end(), m_working, session);
            delete param;
            break;
        }
    }
}
// ----------------------------------------------------------------------------
#endif