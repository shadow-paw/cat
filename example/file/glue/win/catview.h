#ifndef __GLUE_CATVIEW_H__
#define __GLUE_CATVIEW_H__

#include "libcat.h"

namespace cat {
// ----------------------------------------------------------------------------
class CATView {
public:
    CATView();
    ~CATView();

    bool init(const char* title, int width, int height);
    void fini();
    void set_kernel(Kernel* kernel);

    int width() const { return m_width; }
    int height() const { return m_height; }
    HWND hwnd() const { return m_hwnd; }

private:
    Kernel* m_kernel;

private:
    static const int IDT_RENDER = 1;
    int m_width, m_height;
    HWND  m_hwnd;
    HDC   m_hdc;
    HGLRC m_gl;
    UINT_PTR m_timer;
    unsigned int m_mouse_capture;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __GLUE_CATVIEW_H__
