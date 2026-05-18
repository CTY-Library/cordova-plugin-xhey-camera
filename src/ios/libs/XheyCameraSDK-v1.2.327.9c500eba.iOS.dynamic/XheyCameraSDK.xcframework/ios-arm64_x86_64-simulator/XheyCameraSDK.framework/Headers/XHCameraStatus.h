//
//  XHCameraStatus.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/28.
//

#import <Foundation/Foundation.h>
#import "XHCameraDefines.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraStatus : NSObject

@property (nonatomic, assign) BOOL isFrontCamera;

@property (nonatomic, copy) NSArray<NSNumber *> *supportedFlashModes;

@property (nonatomic, assign) XHCameraFlashMode flashMode;

@property (nonatomic, assign) CGFloat maxZoomFactor;

@property (nonatomic, assign) CGFloat minZoomFactor;

@property (nonatomic, assign) CGFloat zoomFactor;

@property (nonatomic, assign) BOOL isManualFocusSupported;

@property (nonatomic, assign) BOOL isManualExposureSupported;

@property (nonatomic, assign, readonly) BOOL hasCameraPermission;

- (NSDictionary *)toDictionary;

@end

NS_ASSUME_NONNULL_END
