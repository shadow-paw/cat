#import "AppDelegate.h"
#import "CatView.h"
#include "bootapp.h"

// ----------------------------------------------------------------------------
@interface AppDelegate()
@property (nonatomic) cat::Kernel* m_kernel;
@end

@implementation AppDelegate
// ----------------------------------------------------------------------------
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}
// ----------------------------------------------------------------------------
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSWindow *window = [[[NSApplication sharedApplication] windows] objectAtIndex:0];
    CatView* view = (CatView*)window.contentViewController.view;

    cat::PlatformSpecificData psd;
    psd.rootview = (__bridge void*)view;
    psd.res_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
    self.m_kernel = new cat::Kernel();
    if (!self.m_kernel->init(psd)) {
        exit(0);
        return;
    }
    [view setKernel:self.m_kernel];
    self.m_kernel->vfs()->mount("/assets/", new cat::FileDriver(psd.res_path, cat::FileDriver::FLAG_READONLY));
    NSArray *searchPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentPath = [searchPaths objectAtIndex:0];
    self.m_kernel->vfs()->mount("/doc/", new cat::FileDriver([documentPath UTF8String], cat::FileDriver::FLAG_WRITABLE));

    self.m_kernel->context_restored();
    self.m_kernel->startup();
    self.m_kernel->run(new app::BootApp());
    [view startAnimation];
}
- (void)applicationWillTerminate:(NSNotification *)aNotification {
    if (self.m_kernel) {
        NSWindow *window = [[[NSApplication sharedApplication] windows] objectAtIndex:0];
        CatView* view = (CatView*)window.contentViewController.view;
        [view stopAnimation];
        [view setKernel:nullptr];
        self.m_kernel->shutdown();
        self.m_kernel->fini();
        delete self.m_kernel;
        self.m_kernel = nullptr;
    }
}
// ----------------------------------------------------------------------------
@end
