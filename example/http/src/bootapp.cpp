#include "bootapp.h"

using namespace app;
using namespace cat;

// ----------------------------------------------------------------------------
BootApp::BootApp() {
    m_counter = 0;
}
// ----------------------------------------------------------------------------
BootApp::~BootApp() {
}
// ----------------------------------------------------------------------------
// App Lifecycle: cb_startup -> cb_resume <-> cb_pause -> cb_shutdown
// ----------------------------------------------------------------------------
// cb_startup is called upon kernel->run(app)
// ----------------------------------------------------------------------------
bool BootApp::cb_startup(Timestamp now) {
    Logger::d("App", "cb_startup");
    return true;
}
// cb_resume is called when the program has resumed
// ----------------------------------------------------------------------------
void BootApp::cb_resume() {
    Logger::d("App", "cb_resume");
    // Http Test: simple request
    {
        HttpRequest req("https://httpbin.org/post");
        req.add_header("foo", "bar");
        req.add_header("foo2", "dumb");
        req.post("Post Data", "text/plain; charset=utf-8");
        auto http_id = kernel()->net()->http_fetch(std::move(req), [](HttpResponse&& res) -> void {
            for (auto it = res.headers.begin(); it != res.headers.end(); ++it) {
                Logger::d("App", "http -> header = %s:%s", it->first.c_str(), it->second.c_str());
            }
            Logger::d("App", "http -> %d - %s", res.code, res.body.ptr());
        });
        // kernel()->net()->http_cancel(http_id);
    }

    // Http Test: json request
    {
        nlohmann::json json = {
            {"key1", 1234.56},
            {"key2", true},
            {"key3", "hello world"},
            {"key4", nullptr},
            {"nested",{
                {"key", 9876}
            }},
            {"list",{1, 0, 2}},
            {"object",{
                {"currency", "USD"},
                {"value", 12.34}
            }}
        };
        HttpRequest req("https://httpbin.org/post");
        req.add_header("foo", "bar");
        req.add_header("foo2", "dumb");
        req.post(json);
        auto http2_id = kernel()->net()->http_fetch(std::move(req), [this](HttpResponse&& res) -> void {
            for (auto it = res.headers.begin(); it != res.headers.end(); ++it) {
                Logger::d("App", "http -> header = %s:%s", it->first.c_str(), it->second.c_str());
            }
            kernel()->vfs()->write("/doc/http.txt", res.body);
            Logger::d("App", "http -> %d - %s", res.code, res.body.ptr());
            auto json = nlohmann::json::parse(res.body.ptr());
            Logger::d("App", "http -> %d - %s", res.code, json.dump(4).c_str());
        });
    }
}
// cb_pause is called when the program is going background
// ----------------------------------------------------------------------------
void BootApp::cb_pause() {
    Logger::d("App", "cb_pause");
}
// cb_shutdown is called after app->exit()
// ----------------------------------------------------------------------------
void BootApp::cb_shutdown(Timestamp now) {
    Logger::d("App", "cb_shutdown");
}
// ----------------------------------------------------------------------------
// OpenGL Context. Everything retained from ResourceManager is managed and
//                 automatically restored upon cb_context_restored()
//                 You only need to handle your own resources here.
// ----------------------------------------------------------------------------
// cb_context_lost is called when the GL context is lost
// you should release any manual created gfx resources here.
// resources retained by resource manager will be auto reloaded by the kernel.
// ----------------------------------------------------------------------------
void BootApp::cb_context_lost() {
    Logger::d("App", "cb_context_lost");
}
// cb_context_restored is called when the GL context is restored
// you should reload any manual created gfx resources here.
// resources retained by resource manager will be auto reloaded by the kernel.
// ----------------------------------------------------------------------------
bool BootApp::cb_context_restored() {
    Logger::d("App", "cb_context_restored");
    return true;
}
// cb_resize is called when the screen is resized, you may adjust ui scale here
// ----------------------------------------------------------------------------
void BootApp::cb_resize(int width, int height) {
    int preferredW, preferredH;
    switch (kernel()->platform()) {
    case Platform::Windows: preferredW = 1280; preferredH = 720; break;
    case Platform::Mac:     preferredW = 1280; preferredH = 720; break;
    case Platform::IOS:     preferredW = 512;  preferredH = 960; break;
    case Platform::Android: preferredW = 512;  preferredH = 960; break;
    default:                preferredW = 512;  preferredH = 960;
    }
    float scaleX = (float)width / preferredW;
    float scaleY = (float)height / preferredH;
    kernel()->ui()->scale(scaleX<scaleY ? scaleX : scaleY);
}
// cb_render is called in the render pipeline
// ----------------------------------------------------------------------------
void BootApp::cb_render(Renderer* r, Timestamp now) {
    Rect2i rect;
    TextStyle style;
    rect.set(10, 10, 100, 40);
    style.color = 0xffff00ff;
    r->draw2d.drawtext(rect, "Hello", style);
}
// ----------------------------------------------------------------------------
bool BootApp::cb_timer(Timestamp now, int msg) {
    Logger::d("App", "cb_timer: %d - time: %llu", msg, now);
    switch (msg) {
    case 1:
        kernel()->time()->post_timer(this, 1, 1000);
        break;
    case 2:
        m_counter ++;
        if (m_counter < 5) {
            kernel()->time()->post_timer(this, 2, 2000);
        } else {
           kernel()->time()->remove_timer(this, 1);
        } break;
    } return true;
}
// ----------------------------------------------------------------------------

