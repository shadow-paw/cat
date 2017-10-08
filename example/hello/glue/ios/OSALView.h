#import <UIKit/UIKit.h>
#include "libosal.h"

@interface OSALView : UIView
- (void) setKernel:(osal::Kernel*)kernel;
- (void) startAnimation;
- (void) stopAnimation;
- (void) drawView:(id)sender;
@end
