//
//  XHDataExtractProcessor.h
//  XheyCameraSDK
//

#import "XHVideoFrame.h"
#import "XHVideoProcessor.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, XHDataExtractTriggerMode) {
    XHDataExtractTriggerModeAlways = 0,
    XHDataExtractTriggerModeRequested = 1,
};

typedef NS_ENUM(NSUInteger, XHDataExtractFrameType) {
    XHDataExtractFrameTypeTexture = 0,
    XHDataExtractFrameTypeRGBA = 1,
    XHDataExtractFrameTypePixelBuffer = 2,
    XHDataExtractFrameTypeBitmap = 3,
};

typedef void (^XHDataExtractCallback)(XHVideoFrame *_Nullable frame, NSError *_Nullable error);

@interface XHDataExtractProcessor : NSObject <XHVideoProcessor>

- (void)setTriggerMode:(XHDataExtractTriggerMode)mode;
- (void)extractOneFrame;
- (void)setFrameType:(XHDataExtractFrameType)type;
- (void)setEnable:(BOOL)enable;
- (void)setDataCallback:(XHDataExtractCallback)callback;

@end

NS_ASSUME_NONNULL_END
