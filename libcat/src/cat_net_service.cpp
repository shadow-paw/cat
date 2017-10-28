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
HTTP_ID NetService::http_fetch(const char* url, const std::unordered_map<std::string, std::string>* headers, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb) {
    return m_http.fetch(url, headers, data, datalen, cb);
}
// ----------------------------------------------------------------------------
bool NetService::http_cancel(HTTP_ID http_id) {
    return m_http.cancel(http_id);
}
// ----------------------------------------------------------------------------
