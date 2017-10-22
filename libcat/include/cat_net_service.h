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

    bool http(const char* url, const void* data, size_t datalen, std::function<void(bool,const uint8_t*,size_t)>);
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
