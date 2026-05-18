//
//  XHCameraViewConfig.h
//  Pods
//
//  Created by leizh007 on 2025/3/26.
//

#import "XHCameraDefines.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, XHCameraConfigMode) {
    XHCameraConfigModePhoto = 0,
    XHCameraConfigModeVideo = 1,
};

@interface XHCameraViewConfig : NSObject

@property(nonatomic, copy) NSString *appid;

@property(nonatomic, copy) NSString *secretKey;

@property(nonatomic, assign) BOOL needPhotoConfirm;

@property(nonatomic, assign) NSInteger maxImageCount;

// 如果设置了，使用这个作为水印；如果没设置，用户可以自己选择水印模版
@property(nonatomic, copy) NSString *groupWatermarkId;

@property(nonatomic, copy) NSString *bundlePath;

@property(nonatomic, assign) XHCameraConfigMode mode;

@property(nonatomic, assign) XHCameraCaptureMode captureMode;

/// 自定义输入项，key-value 对会作为 URL 参数传递给 H5
@property(nonatomic, copy, nullable) NSDictionary<NSString *, NSString *> *customInputItems;

@end

NS_ASSUME_NONNULL_END
