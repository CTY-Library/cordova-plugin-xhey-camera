//
//  XHCameraJsBridge.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/24.
//

#import "XHCameraMiscUtils.h"
#import "XHCameraStatus.h"
#import "XHCapturedImage.h"
#import "XHVideoFrame.h"
#import <CoreLocation/CoreLocation.h>
#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

NS_ASSUME_NONNULL_BEGIN

@class XHCameraJsBridge;

@protocol XHCameraJsBridgeDelegate <NSObject>

- (void)jsBridgeDidCancel:(XHCameraJsBridge *)jsBridge;

- (void)jsBridgeRequestUpdateLocation:(XHCameraJsBridge *)jsBridge;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge requestOssUpload:(NSDictionary *)body;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge didCaptureStillImages:(NSArray<XHCapturedImage *> *)capturedImages;

- (void)jsBridgeDidVueReady:(XHCameraJsBridge *)jsBridge;

- (BOOL)jsBridgeIsFrontCamera:(XHCameraJsBridge *)jsBridge;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge startPreviewWithCompletion:(void (^)(BOOL, NSError *))completion;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge stopPreviewWithCompletion:(void (^)(BOOL, NSError *))completion;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge switchCamraWithCompletion:(void (^)(BOOL, NSError *))completion;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge captureStillImageWithCompletion:(void (^)(UIImage *, NSError *))completion;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge
    captureStillImageWithWatermarks:(NSArray *)watermarks
                         completion:(void (^)(UIImage *, NSError *))completion;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge
    prepareCaptureForCompositeWithCompletion:(void (^)(BOOL success, NSError *_Nullable error))completion;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge
    compositeWatermarksOnCapture:(NSArray *)watermarks
                  truephotoInfos:(NSArray<XHTruePhotoInfo *> *)truephotoInfos
                      completion:(void (^)(UIImage *_Nullable image, NSError *_Nullable error))completion;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge startRecording:(NSArray *)watermarks;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge updateRecordWatermark:(NSArray *)watermarks;

- (void)jsBridgeStopRecording:(XHCameraJsBridge *)jsBridge;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge
    aesUserComment:(NSString *)str
        completion:(void (^)(NSString *, NSError *))completion;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge setFlashMode:(XHCameraFlashMode)mode;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge setZoomFactor:(CGFloat)factor;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge setFocusAndExposureAtPoint:(CGPoint)point;

- (void)jsBridge:(XHCameraJsBridge *)jsBridge setExposureTargetBias:(CGFloat)bias;

- (void)jsBridgeFeedbackButtonDidClick:(XHCameraJsBridge *)jsBridge;

@end

@interface XHCameraJsBridge : NSObject <WKScriptMessageHandler>

@property(nonatomic, weak) WKWebView *webView;

@property(nonatomic, weak) id<XHCameraJsBridgeDelegate> delegate;

- (void)dispose;

- (void)updateDeviceOrientation:(XHCameraDeviceOrientation)orientation;

- (void)updateLocation:(CLLocation *)location permissionStatus:(BOOL)status;

- (void)updateUILayout:(CGFloat)safeAreaTop bottom:(CGFloat)safeAreaBottom;

- (void)updateAzimuth:(double)azimuth;

- (void)reportCameraPageDuration:(NSInteger)durationInMs;

- (void)updateCameraStatus:(XHCameraStatus *)status;

- (void)updateTimestamp:(int64_t)timestamp inChina:(BOOL)inChina;

- (void)updateAddress:(NSString *)address;

@end

NS_ASSUME_NONNULL_END
