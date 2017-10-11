#include "osalview.h"
#include "bootapp.h"

using namespace osal;
using namespace app;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
    OSALView* view = new OSALView();
    if (!view->init("test", 1280, 720)) {
        delete view;
        return 0;
    }

    PlatformSpecificData psd;
    psd.rootview = view->hwnd();
    osal::Kernel* kernel = new osal::Kernel();
	if ( !kernel->init(psd) ) return 0;
    view->set_kernel(kernel);
    // mount our assets
    kernel->vfs()->mount("/assets/", new osal::FileDriver("../"));
    kernel->context_restored();
    kernel->resize(view->width(), view->height());
    kernel->startup();
    kernel->run(new BootApp());
    MSG msg = { 0 };
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    view->set_kernel(nullptr);
    kernel->shutdown();
    kernel->fini();
    delete kernel;
    delete view;
	return 0;
}

