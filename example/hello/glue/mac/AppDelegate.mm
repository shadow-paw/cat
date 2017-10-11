#import "AppDelegate.h"
#import "OSALView.h"
#include "bootapp.h"

// ----------------------------------------------------------------------------
@interface AppDelegate()
@property (nonatomic) osal::Kernel* m_kernel;
@end

@implementation AppDelegate
// ----------------------------------------------------------------------------
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}
// ----------------------------------------------------------------------------
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSWindow *window = [[[NSApplication sharedApplication] windows] objectAtIndex:0];
    OSALView* view = (OSALView*)window.contentViewController.view;

    osal::PlatformSpecificData psd;
    psd.rootview = (__bridge void*)view;
    self.m_kernel = new osal::Kernel();
    if (!self.m_kernel->init(psd)) {
        exit(0);
        return;
    }
    [view setKernel:self.m_kernel];
    self.m_kernel->vfs()->mount("/assets/", new osal::FileDriver([[[NSBundle mainBundle] resourcePath] UTF8String]));
    self.m_kernel->context_restored();
    self.m_kernel->startup();
    self.m_kernel->run(new app::BootApp());
    [view startAnimation];
}
- (void)applicationWillTerminate:(NSNotification *)aNotification {
    if (self.m_kernel) {
        NSWindow *window = [[[NSApplication sharedApplication] windows] objectAtIndex:0];
        OSALView* view = (OSALView*)window.contentViewController.view;
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
