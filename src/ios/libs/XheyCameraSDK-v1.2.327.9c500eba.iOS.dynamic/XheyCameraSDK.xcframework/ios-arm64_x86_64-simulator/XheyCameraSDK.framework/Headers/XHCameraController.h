//
//  XHCameraController.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/24.
//

#import "XHCameraConfig.h"
#import "XHCameraMiscUtils.h"
#import "XHCameraStatus.h"
#import "XHMediaRecorder.h"
#import "XHVideoFrame.h"
#import "XHWatermark.h"
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class XHCameraController;

@protocol XHCameraControllerDelegate <NSObject>

- (void)cameraController:(XHCameraController *)cameraController statusDidChange:(XHCameraStatus *)status;

@end

@interface XHCameraController : NSObject

+ (void)checkCameraAuthorization:(void (^)(BOOL granted))completion;

@property(nonatomic, strong, readonly) XHCameraStatus *cameraStatus;

- (instancetype)initWithConfig:(XHCameraConfig *)config
                  previewLayer:(AVCaptureVideoPreviewLayer *)previewLayer
                      delegate:(id<XHCameraControllerDelegate>)delegate;

- (void)startPreview;

- (void)startPreviewWithCompletion:(nullable void (^)(BOOL success, NSError *error))completion;

- (void)stopPreview;

- (void)stopPreviewWithCompletion:(nullable void (^)(BOOL success, NSError *error))completion;

- (void)switchCamera;

- (void)switchCameraWithCompletion:(nullable void (^)(BOOL success, NSError *error))completion;

- (void)captureStillImageWithConfig:(XHCaptureImageConfig *)config completion:(void (^)(UIImage *, NSError *))completion;

- (void)captureStillImageWithWatermarks:(NSArray<XHWatermark *> *)watermarks
                                 config:(XHCaptureImageConfig *)config
                             completion:(void (^)(UIImage *, NSError *))completion;

- (void)prepareCaptureForCompositeWithConfig:(XHCaptureImageConfig *)config
                                  completion:(void (^)(BOOL success, NSError *_Nullable error))completion;

- (void)compositeWatermarksOnCapture:(NSArray<XHWatermark *> *)watermarks
                      truephotoInfos:(nullable NSArray<XHTruePhotoInfo *> *)truephotoInfos
                              config:(XHCaptureImageConfig *)config
                          completion:(void (^)(UIImage *_Nullable image, NSError *_Nullable error))completion;

- (void)setFlashMode:(XHCameraFlashMode)flashMode;

- (void)setZoomFactor:(CGFloat)factor;

- (void)setFocusAndExposureAtPoint:(CGPoint)point previewAspectRatio:(CGFloat)previewAspectRatio;

- (void)setExposureTargetBias:(float)bias;

- (void)setUIOrientation:(XHCameraDeviceOrientation)orientation;

- (void)startRecord:(CGSize)size
           rotation:(NSInteger)rotation
         watermarks:(NSArray<XHWatermark *> *)watermarks
           delegate:(id<XHMediaRecorderDelegate>)delegate;

- (void)stopRecord;

- (void)updateRecordWatermark:(NSArray<XHWatermark *> *)watermarks;

@end

NS_ASSUME_NONNULL_END
