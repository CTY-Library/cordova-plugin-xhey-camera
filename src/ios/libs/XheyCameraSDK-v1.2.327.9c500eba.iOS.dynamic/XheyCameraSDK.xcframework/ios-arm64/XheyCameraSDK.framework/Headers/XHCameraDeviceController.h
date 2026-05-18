//
//  XHCameraDevice.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/27.
//

#import "XHCameraAFAEController.h"
#import "XHCameraFlashController.h"
#import "XHCameraSnapshotController.h"
#import "XHCameraZoomController.h"
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraDeviceController : NSObject

- (instancetype)initWithDevice:(AVCaptureDevice *)device position:(AVCaptureDevicePosition)position;

- (instancetype)initWithDevice:(AVCaptureDevice *)device
                      position:(AVCaptureDevicePosition)position
                frontUltraWide:(BOOL)frontUltraWide;

@property(nonatomic, readonly, assign) AVCaptureDevicePosition position;

@property(nonatomic, readonly, strong) AVCaptureDevice *device;

@property(nonatomic, readonly, assign) BOOL isFront;

@property(nonatomic, readonly, strong) XHCameraZoomController *zoomController;

@property(nonatomic, readonly, strong) XHCameraAFAEController *afaeController;

@property(nonatomic, readonly, strong) XHCameraFlashController *flashController;

@property(nonatomic, readonly, strong) XHCameraSnapshotController *snapshotController;

@property(nonatomic, strong) dispatch_queue_t dataOutputQueue;

- (void)collectCameraStatus:(XHCameraStatus *)status;

- (void)onCameraDidStartRunning;

@end

NS_ASSUME_NONNULL_END
