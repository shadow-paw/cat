#include "catview.h"

using namespace cat;

// ----------------------------------------------------------------------------
CATView::CATView() {
    m_kernel = nullptr;
    m_hwnd = nullptr;
    m_mouse_capture = 0;
}
// ----------------------------------------------------------------------------
CATView::~CATView() {
    fini();
}
// ----------------------------------------------------------------------------
bool CATView::init_glew() {
    // We need to create a dummy OpenGL 2.1 context before we can setup a 3.3 window
    bool result = false;

    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSEX wc = {0};
    PIXELFORMATDESCRIPTOR pfd = {0};

    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = L"GLEW";
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                               0, 0, 0, 0, NULL, NULL, hInst, NULL);
    if (hwnd == NULL) goto fail;
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    HDC hdc = GetDC(hwnd);
    int PixelFormat;
    if ((PixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0) goto fail;
    if (!SetPixelFormat(hdc, PixelFormat, &pfd)) goto fail;

    // Init GL
    HGLRC gl;
    if (((gl = wglCreateContext(hdc)) == NULL) || !wglMakeCurrent(hdc, gl)) goto fail;
    if (GLEW_OK != glewInit() || !GLEW_VERSION_2_1) return false;
    result = true;

fail:
    if (hwnd) {
        if (hdc) ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
    }
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return result;
}
// ----------------------------------------------------------------------------
bool CATView::init(const char* title, int width, int height) {
    m_mouse_capture = 0;
    if (!init_glew()) return false;

    TCHAR	t_title[256];
	MultiByteToWideChar(CP_UTF8, 0, title, -1, t_title, 256);
    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.hInstance     = hInst;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpfnWndProc   = WindowProc;
    wc.lpszClassName = t_title;
    RegisterClassEx(&wc);

    RECT rc = { 0 };
    SystemParametersInfo (SPI_GETWORKAREA, 0, &rc, 0);
    m_hwnd = CreateWindowEx(0, wc.lpszClassName, t_title, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
            (rc.left+rc.right-width)/2, (rc.top+rc.bottom-height)/2, width, height,
            NULL, NULL, hInst, this);
    if (m_hwnd == NULL) return false;
    m_hdc = GetDC(m_hwnd);
    // Set Pixel Format
    const int format_attrs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        //WGL_STENCIL_BITS_ARB, 8, 
        0
    };
    int format, numFormat;
    PIXELFORMATDESCRIPTOR pfd = {0};
    if (!wglChoosePixelFormatARB(m_hdc, format_attrs, nullptr, 1, &format, (UINT*)&numFormat)) goto fail;
    if (!SetPixelFormat(m_hdc, format, &pfd)) goto fail;
    // Init GL
    const int contextAttrs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };
    if (((m_gl = wglCreateContextAttribsARB(m_hdc, nullptr, contextAttrs)) == NULL) || !wglMakeCurrent(m_hdc, m_gl)) goto fail;

    // Fix up client rect
    GetClientRect(m_hwnd, &rc);
    int win_w = width + width - (rc.right - rc.left);
    int win_h = height + height - (rc.bottom - rc.top);
    SetWindowPos(m_hwnd, NULL, 0, 0, win_w, win_h, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
    // size
    glViewport(0, 0, width, height);
    m_width = width;
    m_height = height;
    // Start render timer
    m_timer = SetTimer(m_hwnd, IDT_RENDER, 10, NULL);
	return true;
fail:
    fini();
    return false;
}
// ----------------------------------------------------------------------------
void CATView::fini() {
    if (m_gl) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(m_gl);
        m_gl = NULL;
    }
    if (m_hdc) {
        ReleaseDC(m_hwnd, m_hdc);
        m_hdc = NULL;
    }
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }
}
// ----------------------------------------------------------------------------
void CATView::set_kernel(Kernel* kernel) {
    m_kernel = kernel;
}
// ----------------------------------------------------------------------------
LRESULT CALLBACK CATView::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    CATView* view;
    switch (msg) {
    case WM_CREATE:
        view = static_cast<CATView*>((void*)((CREATESTRUCT*)lParam)->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)view);
        return 0;
    case WM_DESTROY:
        SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
        PostQuitMessage(0);
        return 0;
    case WM_TIMER:
        switch (wParam) {
        case IDT_RENDER:
            if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
            if (view->m_kernel->timer()) {
                view->m_kernel->render();
                SwapBuffers(view->m_hdc);
            } break;
        } return 0;
    case WM_SIZE: {
        if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
        int width = (int)LOWORD(lParam);
        int height = (int)HIWORD(lParam);
        glViewport(0, 0, width, height);
        if (!view->m_kernel) break;
        view->m_kernel->resize(width, height);
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_PAINT:
        if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
        view->m_kernel->render();
        SwapBuffers(view->m_hdc);
        break;
    case WM_LBUTTONDOWN: {
        if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
        if (view->m_mouse_capture==0) SetCapture(hwnd);
        view->m_mouse_capture++;
        TouchEvent ev;
        ev.type = TouchEvent::EventType::TouchDown;
        ev.pointer_id = 0;
        ev.x = GET_X_LPARAM(lParam);
        ev.y = GET_Y_LPARAM(lParam);
        ev.button = 1<<0;
        view->m_kernel->touch(ev);
        view->m_kernel->render();
        SwapBuffers(view->m_hdc);
        return 0;
    }
    case WM_LBUTTONUP: {
        if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
        view->m_mouse_capture--;
        if (view->m_mouse_capture == 0) ReleaseCapture();
        TouchEvent ev;
        ev.type = TouchEvent::EventType::TouchUp;
        ev.pointer_id = 0;
        ev.x = GET_X_LPARAM(lParam);
        ev.y = GET_Y_LPARAM(lParam);
        ev.button = 1<<0;
        view->m_kernel->touch(ev);
        view->m_kernel->render();
        SwapBuffers(view->m_hdc);
        return 0;
    }
    case WM_RBUTTONDOWN: {
        if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
        if (view->m_mouse_capture == 0) SetCapture(hwnd);
        view->m_mouse_capture++;
        TouchEvent ev;
        ev.type = TouchEvent::EventType::TouchDown;
        ev.pointer_id = 0;
        ev.x = GET_X_LPARAM(lParam);
        ev.y = GET_Y_LPARAM(lParam);
        ev.button = 1<<1;
        view->m_kernel->touch(ev);
        view->m_kernel->render();
        SwapBuffers(view->m_hdc);
        return 0;
    }
    case WM_RBUTTONUP: {
        if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
        view->m_mouse_capture--;
        if (view->m_mouse_capture == 0) ReleaseCapture();
        TouchEvent ev;
        ev.type = TouchEvent::EventType::TouchUp;
        ev.pointer_id = 0;
        ev.x = GET_X_LPARAM(lParam);
        ev.y = GET_Y_LPARAM(lParam);
        ev.button = 1<<1;
        view->m_kernel->touch(ev);
        view->m_kernel->render();
        SwapBuffers(view->m_hdc);
        return 0;
    }
    case WM_MOUSEMOVE: {
        if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
        TouchEvent ev;
        ev.type = TouchEvent::EventType::TouchMove;
        ev.pointer_id = 0;
        ev.x = GET_X_LPARAM(lParam);
        ev.y = GET_Y_LPARAM(lParam);
        ev.button = 0;
        view->m_kernel->touch(ev);
        view->m_kernel->render();
        SwapBuffers(view->m_hdc);
        return 0;
    }
    case WM_MOUSEWHEEL: {
        if ((view = static_cast<CATView*> ((void*)GetWindowLongPtr(hwnd, GWLP_USERDATA))) == NULL) break;
        POINT mouse;
        GetCursorPos(&mouse);
        ScreenToClient(hwnd, &mouse);
        TouchEvent ev;
        ev.type = TouchEvent::EventType::Scroll;
        ev.pointer_id = 0;
        ev.x = mouse.x;
        ev.y = mouse.y;
        ev.scroll = GET_WHEEL_DELTA_WPARAM(wParam);
        ev.button = 0;
        view->m_kernel->touch(ev);
        view->m_kernel->render();
        SwapBuffers(view->m_hdc);
        return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
// ----------------------------------------------------------------------------
