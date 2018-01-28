#include "catview.h"
#include "bootapp.h"
#include <Pathcch.h>

using namespace cat;
using namespace app;

std::string get_exepath() {
    TCHAR exepath[MAX_PATH];
    DWORD len = GetModuleFileName(NULL, exepath, MAX_PATH);
    if (S_OK != PathCchRemoveFileSpec(exepath, len)) return "";
    return StringUtil::make_string(exepath);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
    CATView* view = new CATView();
    if (!view->init("test", 1280, 720)) {
        delete view;
        return 0;
    }

    PlatformSpecificData psd;
    psd.rootview = view->hwnd();
    cat::Kernel* kernel = new cat::Kernel();
	if ( !kernel->init(psd) ) return 0;
    view->set_kernel(kernel);
    // mount our assets
    kernel->vfs()->mount("/assets/", new cat::FileDriver(get_exepath() + "/../"));
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

