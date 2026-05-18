//
//  XHCameraFlashController.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/28.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "XHCameraDefines.h"
#import "XHCameraStatus.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraFlashController : NSObject

- (instancetype)initWithDevice:(AVCaptureDevice *)device position:(AVCaptureDevicePosition)position;

- (void)setPhotoOutput:(AVCapturePhotoOutput *)photoOutput;

@property (nonatomic, assign) XHCameraFlashMode flashMode;

@property (nonatomic, readonly, copy) NSArray<NSNumber *> *supportedFlashModes;

- (void)collectCameraStatus:(XHCameraStatus *)status;

- (void)setFlashMode:(XHCameraFlashMode)flashMode;

- (void)onCameraDidStartRunning;

@end

NS_ASSUME_NONNULL_END
