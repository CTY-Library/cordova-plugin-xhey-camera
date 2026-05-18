//
//  XHCameraConfig.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/24.
//

#import "XHCameraDefines.h"
#import "XHCameraViewConfig.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraConfig : NSObject

@property(nonatomic, assign) BOOL useFrontCamera;

// 宽高比
@property(nonatomic, assign) CGFloat aspectRatio;

@property(nonatomic, assign) XHCameraConfigMode mode;

@property(nonatomic, assign) XHCameraCaptureMode captureMode;

- (NSDictionary *)toDictionary;

+ (instancetype)configFromDictionary:(NSDictionary *)dict;

@end

@interface XHCaptureImageConfig : NSObject

@property(nonatomic, assign) CGSize targetSize;

@property(nonatomic, assign) NSInteger deviceRotation;

@end

NS_ASSUME_NONNULL_END
