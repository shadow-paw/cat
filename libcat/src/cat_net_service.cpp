#include "cat_net_service.h"

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
HTTP_ID NetService::http_fetch(HttpRequest&& request, std::function<void(HttpResponse&&)> cb) {
    return m_http.fetch(std::move(request), cb);
}
// ----------------------------------------------------------------------------
bool NetService::http_cancel(HTTP_ID http_id) {
    return m_http.cancel(http_id);
}
// ----------------------------------------------------------------------------
