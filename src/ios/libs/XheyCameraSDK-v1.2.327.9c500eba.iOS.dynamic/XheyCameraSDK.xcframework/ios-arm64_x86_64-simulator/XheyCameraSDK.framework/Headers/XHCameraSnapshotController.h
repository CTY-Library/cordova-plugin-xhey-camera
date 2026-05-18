//
//  XHCameraSnapshotController.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/4/22.
//

#import <Foundation/Foundation.h>
#import "XHCameraPictureController.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraSnapshotController : NSObject

- (instancetype)initWithDevice:(AVCaptureDevice *)device
                      position:(AVCaptureDevicePosition)position;

- (void)captureSnapshotWithConfig:(XHCameraPictureConfig *)config completion:(XHCameraPictureCompletion)completion;

@property (nonatomic, strong) dispatch_queue_t snapshotQueue;

- (void)onVideoFrameArrived:(CVPixelBufferRef)pixelBuffer;

@end

NS_ASSUME_NONNULL_END
