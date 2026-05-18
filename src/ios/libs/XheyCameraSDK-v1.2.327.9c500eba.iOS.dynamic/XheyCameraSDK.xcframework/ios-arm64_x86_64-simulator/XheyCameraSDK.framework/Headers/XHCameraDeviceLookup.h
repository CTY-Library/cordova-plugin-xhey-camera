//
//  XHCameraDeviceLookup.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/27.
//

#import "XHCameraDeviceController.h"
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraDeviceLookup : NSObject

@property(nonatomic, readonly, assign) BOOL supportFrontUltraWideCamera;

- (XHCameraDeviceController *)createBackCameraDeviceController;

- (XHCameraDeviceController *)createFrontCameraDeviceController;

@end

NS_ASSUME_NONNULL_END
