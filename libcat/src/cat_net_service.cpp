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
bool NetService::http(const char* url, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb) {
    return m_http.http(url, data, datalen, cb);
}
// ----------------------------------------------------------------------------
