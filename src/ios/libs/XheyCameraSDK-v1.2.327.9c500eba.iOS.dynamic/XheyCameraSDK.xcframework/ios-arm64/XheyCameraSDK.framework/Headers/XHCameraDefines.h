//
//  XHCameraDefines.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/28.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, XHCameraFlashMode) {
    XHCameraFlashModeOff = 0,
    XHCameraFlashModeAuto,
    XHCameraFlashModeOn,
    XHCameraFlashModeTorch
};

typedef NS_ENUM(NSInteger, XHCameraCaptureMode) {
    XHCameraCaptureModeSpeedPriority = 0,
    XHCameraCaptureModeQualityPriority,
};

#define kTopToolBarHeight ([XHCameraMiscUtils isFullScreenIPhone] ? 60.0 : 44.0)

NS_ASSUME_NONNULL_END
