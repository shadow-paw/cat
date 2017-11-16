/*
 * Reference: https://developer.apple.com/library/content/samplecode/GLEssentials/Introduction/Intro.html
 */

#import "CatView.h"

// ----------------------------------------------------------------------------
@interface CatView ()
@property (nonatomic) EAGLContext* m_context;
@property (nonatomic) CADisplayLink* m_displayLink;
@property (atomic) NSTimer* m_timer;
@property (nonatomic) BOOL m_animating;
@property (nonatomic) cat::Kernel* m_kernel;
@property (nonatomic) GLuint m_fbo, m_rb_color, m_rb_depth;
@end

// ----------------------------------------------------------------------------
@implementation CatView
// ----------------------------------------------------------------------------
@synthesize m_context;
@synthesize m_displayLink;
@synthesize m_timer;
@synthesize m_animating;
@synthesize m_kernel;
@synthesize m_fbo, m_rb_color, m_rb_depth;
// ----------------------------------------------------------------------------
// Must return the CAEAGLLayer class so that CA allocates an EAGLLayer backing for this view
+ (Class) layerClass {
    return [CAEAGLLayer class];
}
#pragma mark -
#pragma mark Lifecycle
// ----------------------------------------------------------------------------
- (instancetype) initWithCoder:(NSCoder*)coder {
    if ((self = [super initWithCoder:coder])) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                            [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                            kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                            nil];
		self.m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!self.m_context || ![EAGLContext setCurrentContext:self.m_context]) return nil;
        if (![self initFBO]) return nil;
        self.m_animating = FALSE;
        self.m_displayLink = nil;
    }
    return self;
}
// ----------------------------------------------------------------------------
- (void) dealloc {
    [self finiFBO];
    if ([EAGLContext currentContext] == self.m_context) {
        [EAGLContext setCurrentContext:nil];
    }
}
// ----------------------------------------------------------------------------
- (void) setKernel:(cat::Kernel*)kernel {
    self.m_kernel = kernel;
}
#pragma mark -
#pragma mark FBO
// ----------------------------------------------------------------------------
- (BOOL) initFBO {
    glGenFramebuffers(1, &m_fbo);
    glGenRenderbuffers(1, &m_rb_color);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rb_color);
    [self.m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)self.layer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.m_rb_color);
    // Get the drawable buffer's width and height so we can create a depth buffer for the FBO
    GLint backingWidth, backingHeight;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
    // Create a depth buffer to use with our drawable FBO
    glGenRenderbuffers(1, &m_rb_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, self.m_rb_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rb_depth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return FALSE;
    } return TRUE;
}
// ----------------------------------------------------------------------------
- (void) finiFBO {
    if (self.m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        self.m_fbo = 0;
    }
    if (self.m_rb_color) {
        glDeleteRenderbuffers(1, &m_rb_color);
        self.m_rb_color = 0;
    }
    if (self.m_rb_depth) {
        glDeleteRenderbuffers(1, &m_rb_depth);
        self.m_rb_depth = 0;
    }
}
// ----------------------------------------------------------------------------
- (BOOL) resizeFBO {
    GLint backingWidth, backingHeight;
    glBindRenderbuffer(GL_RENDERBUFFER, self.m_rb_color);
    [self.m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)self.layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, self.m_rb_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, self.m_rb_depth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return FALSE;
    } return TRUE;
}
#pragma mark -
#pragma mark Render
// ----------------------------------------------------------------------------
- (void) drawView:(id)sender {
    [EAGLContext setCurrentContext:self.m_context];
    glBindFramebuffer(GL_FRAMEBUFFER, self.m_fbo);
    self.m_kernel->render();
    glBindRenderbuffer(GL_RENDERBUFFER, self.m_rb_color);
    [self.m_context presentRenderbuffer:GL_RENDERBUFFER];
}
// ----------------------------------------------------------------------------
- (void) layoutSubviews {
    [self resizeFBO];
    glViewport(0, 0, self.bounds.size.width, self.bounds.size.height);
    self.m_kernel->resize(self.bounds.size.width, self.bounds.size.height);
    [self drawView:nil];
}
// ----------------------------------------------------------------------------
- (void) startAnimation {
	if (!self.m_animating) {
        self.m_displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        self.m_displayLink.preferredFramesPerSecond = 30;
        // Have the display link run on the default runn loop (and the main thread)
        [self.m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		self.m_animating = TRUE;
	}
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
	if (self.m_animating) {
        [self.m_displayLink invalidate];
        self.m_displayLink = nil;
		self.m_animating = FALSE;
	}
}
// ----------------------------------------------------------------------------
// Timer
// ----------------------------------------------------------------------------
- (void)onTimer:(NSTimer *)timer {
    self.m_kernel->timer();
}
// ----------------------------------------------------------------------------
#pragma mark -
#pragma mark Touch Events
// ----------------------------------------------------------------------------
- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent *)event {
    for (UITouch* touch: touches) {
        CGPoint pos = [touch locationInView:self];
        cat::TouchEvent ev;
        ev.type = cat::TouchEvent::TouchDown;
        ev.pointer_id = 0;
        ev.x = pos.x;
        ev.y = pos.y;
        ev.button = 1;
        self.m_kernel->touch(ev);
    }
}
- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent *)event {
    for (UITouch* touch: touches) {
        CGPoint pos = [touch locationInView:self];
        cat::TouchEvent ev;
        ev.type = cat::TouchEvent::TouchUp;
        ev.pointer_id = 0;
        ev.x = pos.x;
        ev.y = pos.y;
        ev.button = 1;
        self.m_kernel->touch(ev);
    }
}
- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent *)event {
    for (UITouch* touch: touches) {
        CGPoint pos = [touch locationInView:self];
        cat::TouchEvent ev;
        ev.type = cat::TouchEvent::TouchMove;
        ev.pointer_id = 0;
        ev.x = pos.x;
        ev.y = pos.y;
        ev.button = 1;
        self.m_kernel->touch(ev);
    }
}
@end
