//
//  XHCameraZoomController.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/28.
//

#import "XHCameraStatus.h"
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraZoomController : NSObject

- (instancetype)initWithDevice:(AVCaptureDevice *)device position:(AVCaptureDevicePosition)position;

- (instancetype)initWithDevice:(AVCaptureDevice *)device
                      position:(AVCaptureDevicePosition)position
                frontUltraWide:(BOOL)frontUltraWide;

@property(nonatomic, readonly, assign) CGFloat maxZoomFactor;

@property(nonatomic, readonly, assign) CGFloat minZoomFactor;

@property(nonatomic, assign) CGFloat zoomFactor;

- (void)didConnectToSession;

- (void)collectCameraStatus:(XHCameraStatus *)status;

@end

NS_ASSUME_NONNULL_END
