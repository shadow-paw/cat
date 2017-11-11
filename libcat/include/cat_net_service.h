#ifndef __CAT_NET_SERVICE_H__
#define __CAT_NET_SERVICE_H__

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
    HTTP_ID http_fetch(HttpRequest&& request, std::function<void(const HttpResponse&)> cb);
    bool    http_cancel(HTTP_ID http_id);

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
