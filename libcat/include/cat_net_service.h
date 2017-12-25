#ifndef __CAT_NET_SERVICE_H__
#define __CAT_NET_SERVICE_H__

#include "cat_net_http.h"

namespace cat {
class Kernel;
// ----------------------------------------------------------------------------
//! Network Service
class NetService {
friend class cat::Kernel;
public:
    NetService();
    ~NetService();

    //! Start an HTTP session
    //! \param request HTTP request
    //! \param cb Callback upon complete/failure, will be invoked from main thread
    //! \return HTTP id used for cancallation
    //! \sa HttpRequest, cancel
    HTTP_ID http_fetch(HttpRequest&& request, std::function<void(HttpResponse&&)> cb);
    //! Cancel an HTTP session
    //! \param http_id HTTP id obtained from fetch
    //! \return true if cancelled, false if cannot be cancelled
    //! \sa fetch
    bool http_cancel(HTTP_ID http_id);

private:
    //! Initialize service
    bool init();
    //! Cleanup service
    void fini();
    //! Called from kernel when the app is put to background
    void pause();
    //! Called from kernel when the app is resume to foreground
    void resume();
    //! Called from kernel to poll for event
    void poll();

private:
    HttpManager m_http;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_NET_SERVICE_H__
