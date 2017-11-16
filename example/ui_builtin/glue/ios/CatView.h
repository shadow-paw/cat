#import <UIKit/UIKit.h>
#include "libcat.h"

@interface CatView : UIView
- (void) setKernel:(cat::Kernel*)kernel;
- (void) startAnimation;
- (void) stopAnimation;
- (void) drawView:(id)sender;
@end
