#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#include "libcat.h"

@interface CatView : NSOpenGLView {
}
- (void) setKernel:(cat::Kernel*)kernel;
- (void) startAnimation;
- (void) stopAnimation;
@end

