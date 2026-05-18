//
//  XHCameraController.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/1/23.
//

#import "XHCameraViewConfig.h"
#import "XHCapturedImage.h"
#import "XHMediaRecorder.h"
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class XHCameraViewController;

@protocol XHCameraViewControllerDelegate <NSObject>

// 每项包含 JPEG 图片数据和对应的 UserComment 元数据
- (void)cameraViewController:(XHCameraViewController *)cameraViewController
       didCaptureStillImages:(NSArray<XHCapturedImage *> *)capturedImages;

- (void)cameraViewController:(XHCameraViewController *)cameraViewController didFinishRecording:(XHVideoInfo *)videoInfo;

- (void)cameraViewControllerDidCancel:(XHCameraViewController *)cameraViewController;

- (void)cameraViewControllerWillDismiss:(XHCameraViewController *)cameraViewController;

- (void)cameraViewController:(XHCameraViewController *)cameraViewController didFailWithError:(NSError *)error;

- (void)cameraViewControllerDidClickFeedback:(XHCameraViewController *)cameraViewController;

@end

@interface XHCameraViewController : UIViewController

@property(nonatomic, readonly, copy) NSArray<XHCapturedImage *> *capturedImages;

- (instancetype)initWithConfig:(XHCameraViewConfig *)config delegate:(id<XHCameraViewControllerDelegate>)delegate;

- (void)dimiss;

@end

NS_ASSUME_NONNULL_END
