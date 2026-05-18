//
//  XHCameraMiscUtils.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/23.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, XHCameraDeviceOrientation) {
    XHCameraDeviceOrientationPortrait = 1,   // 竖屏
    XHCameraDeviceOrientationDown = 2,       // 倒置竖屏
    XHCameraDeviceOrientationLeft = 3,       // 左向横屏
    XHCameraDeviceOrientationRight = 4,      // 右向横屏
    XHCameraDeviceOrientationUnknown = 5     // 未知方向
};

@interface XHCameraMiscUtils : NSObject

+ (NSDictionary *)generateSignatureWithAppid:(NSString *)appid secretKey:(NSString *)secret;

+ (void)updateOssRequest:(NSDictionary *)body;

+ (NSInteger)deviceMajorVersion;

+ (NSInteger)deviceMinorVersion;

+ (void)swizzleTargetClass:(Class)targetClass
                 targetSel:(NSString *)targetSel
                  srcClass:(Class)srcClass
                    srcSel:(NSString *)srcSel;

+ (UIEdgeInsets)getSafeAreaEdges;

+ (BOOL)isFullScreenIPhone;

+ (AVCaptureVideoOrientation)videoOrientationFromDeviceOrientation:(XHCameraDeviceOrientation)orientation;

+ (UIImageOrientation)imageOrientationFromDeviceOrientation:(XHCameraDeviceOrientation)orientation;

@end

#ifndef weakify
#if __has_feature(objc_arc)
#define weakify(object) try{} @finally{} {} __weak __typeof__(object) weak##_##object = object;
#else
#define weakify(object) try{} @finally{} {} __block __typeof__(object) block##_##object = object;
#endif
#endif

#ifndef strongify
#if __has_feature(objc_arc)
#define strongify(object) try{} @finally{} __strong __typeof__(object) object = weak##_##object;
#else
#define strongify(object) try{} @finally{} __strong __typeof__(object) object = block##_##object;
#endif
#endif

#define metamacro_concat_(A, B) A ## B

#define metamacro_concat(A, B) metamacro_concat_(A, B)

typedef void (^xhc_cleanupBlock_t)(void);


#ifdef __cplusplus
extern "C" {
#endif

void xhc_executeCleanupBlock (__strong xhc_cleanupBlock_t _Nonnull * _Nullable block);

#ifdef __cplusplus
}
#endif

#define onExit \
    try {} @finally {} \
    __strong xhc_cleanupBlock_t metamacro_concat(mtl_exitBlock_, __LINE__) __attribute__((cleanup(xhc_executeCleanupBlock), unused)) = ^

NS_ASSUME_NONNULL_END
