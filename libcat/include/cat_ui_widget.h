#ifndef __CAT_UI_WIDGET_H__
#define __CAT_UI_WIDGET_H__

#include <vector>
#include <string>
#include "cat_platform.h"
#include "cat_type.h"
#include "cat_kernel_api.h"
#include "cat_gfx_type.h"
#include "cat_gfx_texref.h"
#include "cat_gfx_draw2d.h"
#include "cat_time_queue.h"
#include "cat_ui_handler.h"
#include "cat_ui_animator.h"

namespace cat {
// ----------------------------------------------------------------------------
class Widget : public TimerHandler<int> {
friend class UIService;
public:
    // Event Handlers
    UIHandler<> ev_click;
    struct ANIMATORS {
        ANIMATORS(Widget* w) : translate(w), opacity(w) {}
        TranslateAnimator translate;
        OpacityAnimator opacity;
    } animators;

    Widget(KernelApi* kernel, const Rect2i& rect, unsigned int id = 0);
    virtual ~Widget();

    // ------------------------------------------------------------------ Size and Position
    void set_pos(int x, int y);
    void set_size(int width, int height);
    void set_pos(const Point2i& pos);
    void set_size(const Size2i& size);
    const Size2i& get_size() const { return m_rect.size; }
    void bring_tofront();
    // ------------------------------------------------------------------ Size and Position

    // ------------------------------------------------------------------ Flags
    void set_visible(bool b);
    bool is_visible() const { return m_visible; }
    void set_enable(bool b);
    bool is_enabled() const { return m_enable; }
    void set_clipping(bool clipping);
    bool is_clipping() const { return m_clipping; }
    // ------------------------------------------------------------------ Flags

    // ------------------------------------------------------------------ Visual
    void     set_bgcolor(uint32_t color);
    uint32_t get_bgcolor() const { return m_bgcolor; }
    void     set_opacity(float opacity);
    float    get_opacity() const { return m_opacity; }
    void     set_texture(unsigned int index, const std::string& name, int u0, int v0, int u1, int v1, int border_u = 0, int border_v = 0);
    void     set_texture(unsigned int index, const char* name, int u0, int v0, int u1, int v1, int border_u = 0, int border_v = 0);
    // ------------------------------------------------------------------ Visual

    // ------------------------------------------------------------------ Actions
    bool perform_click();
    // ------------------------------------------------------------------ Actions

    // ------------------------------------------------------------------ Child
    bool attach(Widget* child);
    void detach(Widget* child);
    void remove_childs();
    Widget* child_at(unsigned int index) const;
    Widget* child_withid(unsigned int id) const;
    // ------------------------------------------------------------------ Child

protected:
    virtual void cb_uiscale() {}
    virtual void cb_visible(bool b) {}
    virtual void cb_enable(bool b) {}
    virtual void cb_resize() {}
    virtual bool cb_timer(Timestamp now, int code) { return false; }
    virtual bool cb_touch(const TouchEvent& ev, bool handled) { return false; }
    virtual void cb_render(Renderer* r, Timestamp now) {}

protected:  // Helper function for widget
    void update_absrect();
    void update_absopacity(float parent_absopacity);
    void post_timer(Timestamp delay, int code);
    void remove_timer();
    void capture(Texture& tex, const Rect2i& rect);
    uint32_t apply_opacity(uint32_t color) const;

protected:
    Widget* m_parent;
    std::vector<Widget*> m_childs;
    unsigned int m_id;
    Rect2i       m_rect, m_absrect;
    bool         m_clipping;
    uint32_t     m_bgcolor;
    float        m_opacity, m_absopacity;
    bool         m_enable, m_visible;
    std::vector<TextureRef> m_texrefs;
private:
    // called from UISystem or internal signal propagate
    void render(Renderer* r, Timestamp now);
    bool touch(const TouchEvent& ev, bool handled);
    void notify_uiscaled();
    void notify_visible(bool b);
    void notify_enable(bool b);

protected:
    KernelApi* kernel() const { return m_kernel; }
    Draw2D* draw2d();
    void dirty();
private:
    KernelApi* m_kernel; // hide from widget, access throw helper
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_WIDGET_H__



