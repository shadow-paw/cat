#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#include "libosal.h"

@interface OSALView : NSOpenGLView {
}
- (void) setKernel:(osal::Kernel*)kernel;
- (void) startAnimation;
- (void) stopAnimation;
@end

