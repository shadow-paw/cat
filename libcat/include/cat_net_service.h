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
    HTTP_ID http_fetch(const std::string& url,
                       std::function<void(int, const uint8_t*, size_t)> cb) {
        return http_fetch(url, std::unordered_multimap<std::string, std::string>(), Buffer(), cb);
    }
    HTTP_ID http_fetch(const std::string& url,
                       std::unordered_multimap<std::string, std::string>&& headers,
                       std::function<void(int, const uint8_t*, size_t)> cb) {
        return http_fetch(url, std::move(headers), Buffer(), cb);
    }
    HTTP_ID http_fetch(const std::string& url,
                       Buffer&& data,
                       std::function<void(int, const uint8_t*, size_t)> cb) {
        return http_fetch(url, std::unordered_multimap<std::string, std::string>(), std::move(data), cb);
    }
    HTTP_ID http_fetch(const std::string& url,
                       const std::string& data,
                       std::function<void(int, const uint8_t*, size_t)> cb) {
        return http_fetch(url, std::unordered_multimap<std::string, std::string>(), Buffer(data.c_str(), data.length()), cb);
    }
    HTTP_ID http_fetch(const std::string& url,
                       std::unordered_multimap<std::string, std::string>&& headers,
                       const std::string& data,
                       std::function<void(int, const uint8_t*, size_t)> cb) {
        return http_fetch(url, std::move(headers), Buffer(data.c_str(), data.length()), cb);
    }
    HTTP_ID http_fetch(const std::string& url,
                       std::unordered_multimap<std::string, std::string>&& headers,
                       Buffer&& data,
                       std::function<void(int, const uint8_t*, size_t)> cb);
    bool http_cancel(HTTP_ID http_id);

private:
    // called from kernel
    void pause();
    void resume();
    void poll();
private:
    HttpManager m_http;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_NET_SERVICE_H__
