#import "AppDelegate.h"
#import "OSALView.h"
#include "app.h"

// ----------------------------------------------------------------------------
@interface AppDelegate()
@property (nonatomic) app::AppKernel* m_kernel;
@end

@implementation AppDelegate
// ----------------------------------------------------------------------------
@synthesize m_kernel;
// ----------------------------------------------------------------------------
#pragma mark -
#pragma mark Lifecycle
// ----------------------------------------------------------------------------
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [self.window makeKeyAndVisible];
    OSALView* view = (OSALView*)self.window.rootViewController.view;
    osal::PlatformSpecificData psd;
    psd.rootview = (__bridge void*)view;
    self.m_kernel = new app::AppKernel();
    if (!self.m_kernel->init(psd)) return NO;
    [view setKernel:self.m_kernel];
    self.m_kernel->context_restored();
    self.m_kernel->startup();
	[view startAnimation];
    return YES;
}
// ----------------------------------------------------------------------------
- (void)applicationWillResignActive:(UIApplication *)application {
	[(OSALView*)self.window.rootViewController.view stopAnimation];
}
// ----------------------------------------------------------------------------
- (void)applicationDidEnterBackground:(UIApplication *)application {
	[(OSALView*)self.window.rootViewController.view stopAnimation];
    self.m_kernel->pause();
}
// ----------------------------------------------------------------------------
- (void)applicationWillEnterForeground:(UIApplication *)application {
	[(OSALView*)self.window.rootViewController.view startAnimation];
}
// ----------------------------------------------------------------------------
- (void)applicationDidBecomeActive:(UIApplication *)application {
    self.m_kernel->resume();
	[(OSALView*)self.window.rootViewController.view startAnimation];
}
// ----------------------------------------------------------------------------
- (void)applicationWillTerminate:(UIApplication *)application {
    OSALView* view = (OSALView*)self.window.rootViewController.view;
    [view stopAnimation];
    [view setKernel:nullptr];
    self.m_kernel->shutdown();
    self.m_kernel->fini();
    delete self.m_kernel;
    self.m_kernel = nullptr;
}
// ----------------------------------------------------------------------------
#pragma mark -
#pragma mark Memory management
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
}
// ----------------------------------------------------------------------------
@end
