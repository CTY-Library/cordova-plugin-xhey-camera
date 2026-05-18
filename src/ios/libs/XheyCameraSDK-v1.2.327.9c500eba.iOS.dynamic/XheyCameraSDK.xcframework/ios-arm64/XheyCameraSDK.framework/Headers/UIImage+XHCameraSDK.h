//
//  UIImage+XHCameraSDK.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/28.
//

#import <CoreImage/CoreImage.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIImage (XHCameraSDK)

- (UIImage *)imageByCorrectingOrientation;

- (UIImage *)imageByMirroring;

- (UIImage *)imageByScale:(CGFloat)scale;

- (UIImage *)imageByScaleToTargetSize:(CGSize)targetSize;

+ (UIImage *)scaledAndRotatedUIImageFromPixelBuffer:(CVPixelBufferRef)pixelBuffer
                                         targetSize:(CGSize)targetSize
                                        orientation:(UIImageOrientation)orientation
                                          ciContext:(CIContext *)ciContext;

+ (UIImage *)imageFromPixelBuffer:(CVPixelBufferRef)pixelBuffer ciContext:(CIContext *)ciContext;

- (NSData *)rgbaPixels;

+ (nullable UIImage *)imageWithRGBAData:(NSData *)data width:(int)width height:(int)height;

@end

NS_ASSUME_NONNULL_END
