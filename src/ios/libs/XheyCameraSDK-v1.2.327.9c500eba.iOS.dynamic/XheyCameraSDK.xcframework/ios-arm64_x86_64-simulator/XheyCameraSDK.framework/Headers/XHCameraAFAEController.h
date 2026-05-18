//
//  XHCameraAFAEController.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/28.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "XHCameraStatus.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraAFAEController : NSObject

- (instancetype)initWithDevice:(AVCaptureDevice *)device position:(AVCaptureDevicePosition)position;

- (void)collectCameraStatus:(XHCameraStatus *)status;

- (void)setFocusAndExposureAtPoint:(CGPoint)point;

- (void)setExposureTargetBias:(float)bias;

@end

NS_ASSUME_NONNULL_END
