//
//  XHPixelBufferWrapper.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHMediaKitDefs.h"
#import <CoreVideo/CVPixelBuffer.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHPixelBufferWrapper : NSObject

@property(nonatomic, assign) CVPixelBufferRef pixelBuffer;

- (instancetype)initWithPixelBuffer:(CVPixelBufferRef)pixelBuffer;

@property(nonatomic, readonly) XHPixelbufferType pixelbufferType;

@property(nonatomic, readonly) XHColorRange colorRange;

@property(nonatomic, readonly) XHColorSpace colorSpace;

@property(nonatomic, readonly) int32_t width;

@property(nonatomic, readonly) int32_t height;

@end

NS_ASSUME_NONNULL_END
