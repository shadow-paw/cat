#include "cat_net_service.h"
#include "cat_util_string.h"

using namespace cat;

// ----------------------------------------------------------------------------
NetService::NetService() {
}
// ----------------------------------------------------------------------------
NetService::~NetService() {
}
// ----------------------------------------------------------------------------
void NetService::pause() {
    m_http.pause();
}
// ----------------------------------------------------------------------------
void NetService::resume() {
    m_http.resume();
}
// ----------------------------------------------------------------------------
void NetService::poll() {
    m_http.poll();
}
// ----------------------------------------------------------------------------
HTTP_ID NetService::http_fetch(const std::string& url,
                               std::unordered_multimap<std::string, std::string>&& headers,
                               Buffer&& data,
                               std::function<void(int, const uint8_t*, size_t)> cb) {
    return m_http.fetch(url, std::move(headers), std::move(data), cb);
}
// ----------------------------------------------------------------------------
bool NetService::http_cancel(HTTP_ID http_id) {
    return m_http.cancel(http_id);
}
// ----------------------------------------------------------------------------
