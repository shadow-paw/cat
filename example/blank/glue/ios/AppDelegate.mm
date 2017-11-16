#import "AppDelegate.h"
#import "CatView.h"
#include "bootapp.h"

// ----------------------------------------------------------------------------
@interface AppDelegate()
@property (nonatomic) cat::Kernel* m_kernel;
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
    CatView* view = (CatView*)self.window.rootViewController.view;
    cat::PlatformSpecificData psd;
    psd.rootview = (__bridge void*)view;
    self.m_kernel = new cat::Kernel();
    if (!self.m_kernel->init(psd)) return NO;
    [view setKernel:self.m_kernel];
    self.m_kernel->vfs()->mount("/assets/", new cat::FileDriver([[[NSBundle mainBundle] resourcePath] UTF8String]));
    self.m_kernel->context_restored();
    self.m_kernel->startup();
    self.m_kernel->run(new app::BootApp());
	[view startAnimation];
    return YES;
}
// ----------------------------------------------------------------------------
- (void)applicationWillResignActive:(UIApplication *)application {
	[(CatView*)self.window.rootViewController.view stopAnimation];
}
// ----------------------------------------------------------------------------
- (void)applicationDidEnterBackground:(UIApplication *)application {
	[(CatView*)self.window.rootViewController.view stopAnimation];
    self.m_kernel->pause();
}
// ----------------------------------------------------------------------------
- (void)applicationWillEnterForeground:(UIApplication *)application {
	[(CatView*)self.window.rootViewController.view startAnimation];
}
// ----------------------------------------------------------------------------
- (void)applicationDidBecomeActive:(UIApplication *)application {
    self.m_kernel->resume();
	[(CatView*)self.window.rootViewController.view startAnimation];
}
// ----------------------------------------------------------------------------
- (void)applicationWillTerminate:(UIApplication *)application {
    CatView* view = (CatView*)self.window.rootViewController.view;
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
