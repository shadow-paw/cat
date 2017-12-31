#ifndef __CAT_UI_EDIT_H__
#define __CAT_UI_EDIT_H__

#include "cat_ui_widget.h"

namespace cat {
// ----------------------------------------------------------------------------
class Editbox : public Widget {
public:
    static const int TexBackground = 0;

    Editbox(KernelApi* kernel_api, const Rect2i& rect, unsigned int id = 0);
    virtual ~Editbox();

    void             set_text(const std::string& s);
    std::string      get_text() const;
    void             set_textstyle(const TextStyle& style);
    const TextStyle& get_textstyle() const { return m_textstyle; }
    void             set_textsize(int size);
    int              get_textsize() const { return m_textstyle.fontsize; }
    void             set_textcolor(uint32_t color);
    uint32_t         get_textcolor() const { return m_textstyle.color; }

protected:
    virtual void cb_visible(bool b);
    virtual void cb_resize();
    virtual bool cb_touch(const TouchEvent& ev, bool handled);
    virtual void cb_render(Renderer* r, unsigned long now);

protected:
    TextStyle m_textstyle;
    void update_font();
    void update_textcolor();

private:
    std::string m_text;
    bool m_native_show;
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    HWND m_native_ctrl;
    HFONT m_font;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    void* m_native_ctrl;
#elif defined(PLATFORM_ANDROID)
    jobject m_native_ctrl;
#else
    #error Not Implemented!
#endif
    void native_show(bool show);
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_UI_EDIT_H__
