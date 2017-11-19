/*
 * Reference: https://developer.apple.com/library/content/samplecode/GLEssentials/Introduction/Intro.html
 */

#import "CatView.h"

// #define SUPPORT_RETINA_RESOLUTION 1

// ----------------------------------------------------------------------------
@interface CatView()
@property (nonatomic) CVDisplayLinkRef m_displayLink;
@property (atomic) NSTimer* m_timer;
@property (atomic) cat::Kernel* m_kernel;
@end

// ----------------------------------------------------------------------------
@implementation CatView
// ----------------------------------------------------------------------------
@synthesize m_displayLink;
// ----------------------------------------------------------------------------
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp* now,
                                      const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut,
                                      void* displayLinkContext) {
    return [(__bridge CatView*)displayLinkContext getFrameForTime:outputTime];
}
// ----------------------------------------------------------------------------
- (void) windowWillClose:(NSNotification*)notification {
    CVDisplayLinkStop(self.m_displayLink);
}
// ----------------------------------------------------------------------------
- (void) awakeFromNib {
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        // OpenGL 3.2
        //NSOpenGLPFAOpenGLProfile,
        //NSOpenGLProfileVersion3_2Core,
        0
    };
    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if (!pf) NSLog(@"No OpenGL pixel format");
    NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
#if defined(DEBUG)
    CGLEnable([context CGLContextObj], kCGLCECrashOnRemovedFunctions);
#endif
    [self setPixelFormat:pf];
    [self setOpenGLContext:context];
#if SUPPORT_RETINA_RESOLUTION
    [self setWantsBestResolutionOpenGLSurface:YES];
#endif
}
// ----------------------------------------------------------------------------
- (id)init {
    self = [super init];
    [[self window] setAcceptsMouseMovedEvents: YES];
    return self;
}
// ----------------------------------------------------------------------------
- (void) setKernel:(cat::Kernel*)kernel {
    self.m_kernel = kernel;
    if (self.m_kernel) {
        CGLLockContext([[self openGLContext] CGLContextObj]);
        NSRect viewRectPoints = [self bounds];
#if SUPPORT_RETINA_RESOLUTION
        NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
#else
        NSRect viewRectPixels = viewRectPoints;
#endif
        self.m_kernel->resize(viewRectPixels.size.width, viewRectPixels.size.height);
        CGLUnlockContext([[self openGLContext] CGLContextObj]);
     }
}
#pragma mark -
#pragma mark Render
// ----------------------------------------------------------------------------
- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime {
    @autoreleasepool {
        [[self openGLContext] makeCurrentContext];
        CGLLockContext([[self openGLContext] CGLContextObj]);
        self.m_kernel->render();
        CGLFlushDrawable([[self openGLContext] CGLContextObj]);
        CGLUnlockContext([[self openGLContext] CGLContextObj]);
    } return kCVReturnSuccess;
}
// ----------------------------------------------------------------------------
- (void) startAnimation {
    CVDisplayLinkCreateWithActiveCGDisplays(&m_displayLink);
    CVDisplayLinkSetOutputCallback(self.m_displayLink, &MyDisplayLinkCallback, (__bridge void*)self);
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(self.m_displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(self.m_displayLink);
    self.m_timer =[NSTimer scheduledTimerWithTimeInterval:0.033 target:self selector:@selector(onTimer:) userInfo:nil repeats:YES ];
    NSRunLoop * rl = [NSRunLoop mainRunLoop];
    [rl addTimer:self.m_timer forMode:NSRunLoopCommonModes];
}
// ----------------------------------------------------------------------------
- (void) stopAnimation {
    if (self.m_timer) {
        [self.m_timer invalidate];
        self.m_timer = nil;
    }
    if (self.m_displayLink) {
        CVDisplayLinkStop(self.m_displayLink);
        CVDisplayLinkRelease(self.m_displayLink);
        self.m_displayLink = nullptr;
    }
}
// ----------------------------------------------------------------------------
- (void) prepareOpenGL {
    [super prepareOpenGL];
    GLint swapInt = 1;
    [[self openGLContext] makeCurrentContext];
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowWillClose:)
                                                 name:NSWindowWillCloseNotification
                                               object:[self window]];
}
// ----------------------------------------------------------------------------
- (void)renewGState {
    [[self window] disableScreenUpdatesUntilFlush];
    [super renewGState];
}
// ----------------------------------------------------------------------------
- (void)reshape {
    [super reshape];
    if (self.m_kernel) {
        CGLLockContext([[self openGLContext] CGLContextObj]);
        NSRect viewRectPoints = [self bounds];
#if SUPPORT_RETINA_RESOLUTION
        NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
#else
        NSRect viewRectPixels = viewRectPoints;
#endif
        glViewport(0, 0, viewRectPixels.size.width, viewRectPixels.size.height);
        self.m_kernel->resize(viewRectPixels.size.width, viewRectPixels.size.height);
        CGLUnlockContext([[self openGLContext] CGLContextObj]);
    }
}
// ----------------------------------------------------------------------------
// Timer
// ----------------------------------------------------------------------------
- (void)onTimer:(NSTimer *)timer {
    self.m_kernel->timer();
}
// ----------------------------------------------------------------------------
// Input Events
// ----------------------------------------------------------------------------
- (BOOL)acceptsFirstResponder {
    return YES;
}
// ----------------------------------------------------------------------------
- (void)mouseMoved:(NSEvent *)theEvent {
    NSPoint pos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self bounds];
    cat::TouchEvent ev;
    ev.type = cat::TouchEvent::TouchMove;
    ev.pointer_id = 0;
    ev.x = pos.x;
    ev.y = (int)(r.size.height - pos.y);
    ev.button = 0;
    self.m_kernel->touch(ev);
    // [[self nextResponder] mouseMoved:theEvent];
}
// ----------------------------------------------------------------------------
- (void)mouseDown:(NSEvent *)theEvent {
    NSPoint pos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self bounds];
    cat::TouchEvent ev;
    ev.type = cat::TouchEvent::TouchDown;
    ev.pointer_id = 0;
    ev.x = pos.x;
    ev.y = (int)(r.size.height - pos.y);
    ev.button = 1;
    self.m_kernel->touch(ev);
    // [[self nextResponder] mouseDown:theEvent];
}
// ----------------------------------------------------------------------------
- (void)mouseUp:(NSEvent *)theEvent {
    NSPoint pos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self bounds];
    cat::TouchEvent ev;
    ev.type = cat::TouchEvent::TouchUp;
    ev.pointer_id = 0;
    ev.x = pos.x;
    ev.y = (int)(r.size.height - pos.y);
    ev.button = 1;
    self.m_kernel->touch(ev);
    // [[self nextResponder] mouseUp:theEvent];
}
// ----------------------------------------------------------------------------
- (void)rightMouseDown:(NSEvent *)theEvent {
    NSPoint pos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self bounds];
    cat::TouchEvent ev;
    ev.type = cat::TouchEvent::TouchDown;
    ev.pointer_id = 0;
    ev.x = pos.x;
    ev.y = (int)(r.size.height - pos.y);
    ev.button = 2;
    self.m_kernel->touch(ev);
    // [[self nextResponder] rightMouseDown:theEvent];
}
// ----------------------------------------------------------------------------
- (void)rightMouseUp:(NSEvent *)theEvent {
    NSPoint pos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self bounds];
    cat::TouchEvent ev;
    ev.type = cat::TouchEvent::TouchUp;
    ev.pointer_id = 0;
    ev.x = pos.x;
    ev.y = (int)(r.size.height - pos.y);
    ev.button = 2;
    self.m_kernel->touch(ev);
    // [[self nextResponder] rightMouseUp:theEvent];
}
// ----------------------------------------------------------------------------
- (void)mouseDragged:(NSEvent *)theEvent {
    NSPoint pos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self bounds];
    cat::TouchEvent ev;
    ev.type = cat::TouchEvent::TouchMove;
    ev.pointer_id = 0;
    ev.x = pos.x;
    ev.y = (int)(r.size.height - pos.y);
    ev.button = 1;
    self.m_kernel->touch(ev);
    // [[self nextResponder] mouseDragged:theEvent];
}
// ----------------------------------------------------------------------------
- (void)rightMouseDragged:(NSEvent *)theEvent {
    NSPoint pos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self bounds];
    cat::TouchEvent ev;
    ev.type = cat::TouchEvent::TouchMove;
    ev.pointer_id = 0;
    ev.x = pos.x;
    ev.y = (int)(r.size.height - pos.y);
    ev.button = 2;
    self.m_kernel->touch(ev);
    // [[self nextResponder] rightMouseDragged:theEvent];
}
// ----------------------------------------------------------------------------
- (void)scrollWheel:(NSEvent *)theEvent {
    NSPoint pos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self bounds];
    cat::TouchEvent ev;
    ev.type = cat::TouchEvent::Scroll;
    ev.pointer_id = 0;
    ev.x = pos.x;
    ev.y = (int)(r.size.height - pos.y);
    // ev.scroll.deltaX = (int)[theEvent deltaX];
    ev.scroll = (int) [theEvent deltaY];
    self.m_kernel->touch(ev);
    // [[self nextResponder] scrollWheel:theEvent];
}
// -----------------------------------------------------------
@end

