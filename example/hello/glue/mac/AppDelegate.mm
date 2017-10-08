#import "AppDelegate.h"
#import "OSALView.h"
#include "app.h"

// ----------------------------------------------------------------------------
@interface AppDelegate()
@property (nonatomic) app::AppKernel* m_kernel;
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
    self.m_kernel = new app::AppKernel();
    if (!self.m_kernel->init(psd)) {
        exit(0);
        return;
    }
    [view setKernel:self.m_kernel];
    self.m_kernel->context_restored();
    self.m_kernel->startup();
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
