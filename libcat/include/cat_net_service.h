#ifndef __CAT_NET_SERVICE_H__
#define __CAT_NET_SERVICE_H__

#include "cat_net_type.h"
#include "cat_net_http.h"

namespace cat {
class Kernel;
// ----------------------------------------------------------------------------
class NetService {
friend class cat::Kernel;
public:
    NetService();
    ~NetService();

    // HTTP
    HTTP_ID http_fetch(const char* url, std::function<void(bool, const uint8_t*, size_t)> cb) {
        return http_fetch(url, nullptr, nullptr, 0, cb);
    }
    HTTP_ID http_fetch(const char* url, const std::unordered_map<std::string, std::string>& headers, std::function<void(bool, const uint8_t*, size_t)> cb) {
        return http_fetch(url, &headers, nullptr, 0, cb);
    }
    HTTP_ID http_fetch(const char* url, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb) {
        return http_fetch(url, nullptr, data, datalen, cb);
    }
    HTTP_ID http_fetch(const char* url, const std::unordered_map<std::string, std::string>& headers, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb) {
        return http_fetch(url, &headers, data, datalen, cb);
    }
    bool http_cancel(HTTP_ID http_id);

private:
    // called from kernel
    void pause();
    void resume();
    void poll();
private:
    HttpManager m_http;
    HTTP_ID http_fetch(const char* url, const std::unordered_map<std::string, std::string>* headers, const void* data, size_t datalen, std::function<void(bool, const uint8_t*, size_t)> cb);
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_NET_SERVICE_H__
