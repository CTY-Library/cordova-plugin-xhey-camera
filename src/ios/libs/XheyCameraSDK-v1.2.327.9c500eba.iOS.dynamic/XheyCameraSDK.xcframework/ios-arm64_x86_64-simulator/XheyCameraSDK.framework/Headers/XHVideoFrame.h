//
//  XHVideoFrame.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHPixelBufferWrapper.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, XHVideoFrameType) {
    XHVideoFrameTypeUnknown,
    XHVideoFrameTypeTexture,
    XHVideoFrameTypeBitmap,
    XHVideoFrameTypePixelBuffer,
    XHVideoFrameTypeRGBA,
    XHVideoFrameTypeBGRA,
    XHVideoFrameTypeARGB,
    XHVideoFrameTypeI420,
    XHVideoFrameTypeNV12,
    XHVideoFrameTypeNV21,
    XHVideoFrameTypeJPEGData,
    XHVideoFrameTypeNdkImage,
};

typedef NS_ENUM(NSInteger, XHTruePhotoDataType) {
    XHTruePhotoDataTypeTimestamp = 0,
    XHTruePhotoDataTypeLocation = 1,
    XHTruePhotoDataTypeSecurityCode = 2,
};

typedef NS_ENUM(NSInteger, XHTruePhotoErrorCode) {
    XHTruePhotoErrorCodeSuccess = 0,
    XHTruePhotoErrorCodeTimestampSignatureMismatch = 1,
    XHTruePhotoErrorCodeLocationSignatureMismatch = 2,
    XHTruePhotoErrorCodeSecurityCodeSignatureMismatch = 3,
    XHTruePhotoErrorCodeTimestampCrossCheckFailed = 4,
};

@interface XHTruePhotoInfo : NSObject
@property(nonatomic) XHTruePhotoDataType type;
@property(nonatomic, copy) NSString *data;
@property(nonatomic, copy) NSString *signature;
@end

@interface XHVideoFrameAttributes : NSObject

@property(nonatomic, assign) BOOL fromFrontCamera;

@property(nonatomic, assign) BOOL mirror;

@property(nonatomic, assign) int32_t rotation;

@property(nonatomic, assign) uint32_t outputWidth;

@property(nonatomic, assign) uint32_t outputHeight;

@property(nonatomic, assign) BOOL isCaptured;

@property(nonatomic, assign) XHColorRange colorRange;

@property(nonatomic, assign) XHColorSpace colorSpace;

@property(nonatomic, copy) NSArray<XHTruePhotoInfo *> *truephotoInfos;

@property(nonatomic, assign) XHTruePhotoErrorCode truephotoErrorCode;

@end

@interface XHVideoFrame : NSObject

@property(nonatomic, assign) XHVideoFrameType type;

@property(nonatomic, strong) XHVideoFrameAttributes *attributes;

@property(nonatomic, strong) XHPixelBufferWrapper *pixelBuffer;

@property(nonatomic, assign) XHPixelbufferType pixelbufferType;

@property(nonatomic, strong) NSData *data;

// 单位微妙
@property(nonatomic, assign) int64_t timestamp;

@property(nonatomic, assign) int32_t width;

@property(nonatomic, assign) int32_t height;

- (instancetype)initWithPixelBuffer:(CVPixelBufferRef)pixelBuffer;

- (instancetype)initWithUIImage:(UIImage *)image;

@end

NS_ASSUME_NONNULL_END
