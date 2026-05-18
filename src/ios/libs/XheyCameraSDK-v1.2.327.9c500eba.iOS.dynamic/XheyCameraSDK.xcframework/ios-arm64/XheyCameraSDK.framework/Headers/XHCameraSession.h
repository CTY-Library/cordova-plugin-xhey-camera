//
//  XHCameraSession.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/27.
//

#import "XHCameraConfig.h"
#import "XHCameraDeviceLookup.h"
#import "XHCameraMiscUtils.h"
#import "XHCameraStatus.h"
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class XHCameraSession;
@class XHVideoFrame;

@protocol XHCameraSessionDelegate <NSObject>

- (void)cameraSession:(XHCameraSession *)cameraSession statusDidChange:(XHCameraStatus *)status;

- (void)cameraSession:(XHCameraSession *)cameraSession
    didOutputPixelBuffer:(CVPixelBufferRef)pixelBuffer
           withTimestamp:(int64_t)timestamp;

@end

@interface XHCameraSession : NSObject

@property(nonatomic, readonly) BOOL isFrontCamera;

- (instancetype)initWithConfig:(XHCameraConfig *)config
                  deviceLookup:(XHCameraDeviceLookup *)deviceLookup
                  previewLayer:(AVCaptureVideoPreviewLayer *)previewLayer
                      delegate:(id<XHCameraSessionDelegate>)delegate
                operationQueue:(dispatch_queue_t)operationQueue;

- (void)startPreview;

- (void)startPreviewWithCompletion:(nullable void (^)(BOOL success, NSError *error))completion;

- (void)stopPreview;

- (void)stopPreviewWithCompletion:(nullable void (^)(BOOL success, NSError *error))completion;

- (void)switchCamera;

- (void)switchCameraWithCompletion:(nullable void (^)(BOOL success, NSError *error))completion;

- (void)captureStillImageWithConfig:(XHCaptureImageConfig *)config
                         completion:(void (^)(XHVideoFrame *_Nullable, NSError *_Nullable))completion;

- (void)setFlashMode:(XHCameraFlashMode)flashMode;

- (void)setZoomFactor:(CGFloat)factor;

- (void)setFocusAndExposureAtPoint:(CGPoint)point;

- (void)setExposureTargetBias:(float)bias;

- (void)setUIOrientation:(XHCameraDeviceOrientation)orientation;

@end

NS_ASSUME_NONNULL_END
