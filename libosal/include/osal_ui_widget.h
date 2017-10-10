#ifndef __OSAL_UI_WIDGET_H__
#define __OSAL_UI_WIDGET_H__

#include <list>
#include <vector>
#include <string>
#include "osal_platform.h"
#include "osal_type.h"
#include "osal_kernel_api.h"
#include "osal_gfx_type.h"
#include "osal_gfx_texref.h"
#include "osal_time_queue.h"
#include "osal_ui_handler.h"

namespace osal { namespace ui {
// ----------------------------------------------------------------------------
class Widget : public time::TimerHandler<int> {
friend class UIService;
public:
    // Event Handlers
    EventHandlers<> ev_click;

    Widget(KernelApi* kernel, const gfx::Rect2i& rect, unsigned int id = 0);
    virtual ~Widget();

    // ------------------------------------------------------------------ Size and Position
    void set_pos(int x, int y);
    void set_size(int width, int height);
    void set_pos(const gfx::Point2i& pos);
    void set_size(const gfx::Size2i& size);
    const gfx::Size2i& get_size() const { return m_rect.size; }
    void bring_tofront();
    // ------------------------------------------------------------------ Size and Position

    // ------------------------------------------------------------------ Flags
    void set_visible(bool b);
    bool is_visible() const { return m_visible; }
    void set_enable(bool b);
    bool is_enabled() const { return m_enable; }
    // ------------------------------------------------------------------ Flags

    // ------------------------------------------------------------------ Visual
    void     set_bgcolor(uint32_t color);
    uint32_t get_bgcolor() const { return m_bgcolor; }
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
    virtual bool cb_timer(time::Timestamp now, int code) { return false; }
    virtual bool cb_touch(const TouchEvent& ev, bool handled) { return false; }
    virtual void cb_render(gfx::Renderer* r, time::Timestamp now) {}

protected:  // Helper function for widget
    void update_absrect();
    void post_timer(time::Timestamp delay, int code);

protected:
    Widget* m_parent;
    std::list<Widget*> m_childs;
    unsigned int m_id;
    gfx::Rect2i  m_rect, m_absrect;
    uint32_t     m_bgcolor;
    std::vector<gfx::TextureRef> m_texrefs;
    bool         m_enable, m_visible;

private:
    // called from UISystem or internal signal propagate
    void render(gfx::Renderer* r, unsigned long now);
    bool touch(const TouchEvent& ev, bool handled);
    void notify_uiscaled();
    void notify_visible(bool b);
    void notify_enable(bool b);

protected:
    KernelApi* kernel() const { return m_kernel; }
private:
    KernelApi* m_kernel; // hide from widget, access throw helper
};
// ----------------------------------------------------------------------------
}} // namespace osal::ui

#endif // __OSAL_UI_WIDGET_H__



