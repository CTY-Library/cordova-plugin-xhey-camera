//
//  XHWatermarkProcessor.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHVideoProcessor.h"
#import "XHWatermark.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHWatermarkProcessor : NSObject <XHVideoProcessor>

- (void)addWatermark:(XHWatermark *)watermark;

- (void)setRotation:(int32_t)rotation;

- (void)setMirrorFrontCameraOutput:(bool)mirror;

- (void)autoClearWatermarkAfterCapture:(bool)autoClear;

- (void)clearAllWatermarks;

@end

NS_ASSUME_NONNULL_END
