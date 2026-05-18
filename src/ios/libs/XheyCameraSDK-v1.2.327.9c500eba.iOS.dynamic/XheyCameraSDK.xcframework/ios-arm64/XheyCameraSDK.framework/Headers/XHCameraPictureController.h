//
//  XHCameraPictureController.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/28.
//

#import "XHCameraDefines.h"
#import "XHCameraMiscUtils.h"
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

@class XHVideoFrame;

NS_ASSUME_NONNULL_BEGIN

typedef void (^XHCameraPictureCompletion)(XHVideoFrame *_Nullable frame, NSError *_Nullable error);

@interface XHCameraPictureConfig : NSObject

@property(nonatomic, assign) BOOL isFrontCamera;

@property(nonatomic, assign) CGSize targetSize;

@property(nonatomic, assign) XHCameraFlashMode flashMode;

@property(nonatomic, assign) XHCameraDeviceOrientation uiOrientation;

@property(nonatomic, assign) NSInteger deviceRotation;

@property(nonatomic, strong) dispatch_queue_t operationQueue;

@property(nonatomic, assign) XHCameraCaptureMode captureMode;

@end

API_AVAILABLE(ios(11.0))
@interface XHCameraPictureController : NSObject

@property(nonatomic, readonly, strong) AVCapturePhotoOutput *photoOutput;

- (void)captureStillImageWithConfig:(XHCameraPictureConfig *)config completion:(XHCameraPictureCompletion)completion;

@end

NS_ASSUME_NONNULL_END
