//
//  XHLinearSmoothFilter.h
//  XheyCameraSDK
//
//  Created by 赵品均 on 2025/2/10.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHLinearSmoothFilter : NSObject
// gravity[x,y] 0.97/0.0
// 2s
// 0.2 - 7/20 v
// 0.3 - 4/20 v
// 0.4 - 3/20 v
// 0.5 - 2/20 v
// 1s
// 0.2 - 5/10 x
// 0.3 - 4/10 v
// 0.4 - 3/10 v
// 0.5 - 2/10 v
@property (assign) double factor;

- (double)smoothFilter:(double)value;

- (void)reset;

@end

NS_ASSUME_NONNULL_END
