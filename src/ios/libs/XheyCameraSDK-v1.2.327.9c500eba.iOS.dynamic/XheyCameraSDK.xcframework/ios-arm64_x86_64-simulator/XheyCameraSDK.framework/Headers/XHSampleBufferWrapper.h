//
//  XHSampleBufferWrapper.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import <CoreMedia/CMSampleBuffer.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHSampleBufferWrapper : NSObject

@property(nonatomic, assign) CMSampleBufferRef sampleBuffer;

- (instancetype)initWithSampleBuffer:(CMSampleBufferRef)sampleBuffer;

@end

NS_ASSUME_NONNULL_END
