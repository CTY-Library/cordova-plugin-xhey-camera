//
//  XHMediaKitDefs.h
//  Pods
//
//  Created by leizh007 on 2025/11/3.
//

#import <Foundation/Foundation.h>
#import <OpenGLES/EAGL.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, ProcessorGroupType) {
    ProcessorGroupTypeCpu = 0,
    ProcessorGroupTypeGpu = 1,
    ProcessorGroupTypePreview = 2,
    ProcessorGroupTypeRecord = 3,
    ProcessorGroupTypeCaptureImage = 4,
    ProcessorGroupTypeAudio = 5,
};

typedef NS_ENUM(NSUInteger, XHPixelbufferType) {
    XHPixelbufferTypeUnknown,
    XHPixelbufferType420YpCbCr8BiPlanar,
    XHPixelbufferType420YpCbCr8Planar,
    XHPixelbufferType32BGRA,
    XHPixelbufferType420YpCbCr10BiPlanar,
};

typedef NS_ENUM(NSUInteger, XHColorRange) {
    XHColorRangeUnknown,
    XHColorRangeLimited,
    XHColorRangeFull,
};

typedef NS_ENUM(NSUInteger, XHColorSpace) {
    XHColorSpaceUnknown,
    XHColorSpaceBT601,
    XHColorSpaceBT709,
    XHColorSpaceBT2020,
};

@interface XHMediaKitConfig : NSObject

@property(nonatomic, assign) BOOL enableCpuThread;

@property(nonatomic, assign) BOOL enableMediaRecorder;

@property(nonatomic, assign) BOOL enableCaptureImageThread;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
@property(nonatomic, strong, nullable) EAGLSharegroup *sharegroup;
#pragma clang diagnostic pop

@end

NS_ASSUME_NONNULL_END
