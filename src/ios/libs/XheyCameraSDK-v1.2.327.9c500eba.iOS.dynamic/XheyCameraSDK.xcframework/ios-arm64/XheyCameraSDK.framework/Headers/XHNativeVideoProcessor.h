//
//  XHNativeVideoProcessor.h
//  Pods-XHCameraDemo
//
//  Created by leizh007 on 2025/11/3.
//

#include "common/media_types.h"
#include "processor/processor.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol XHNativeVideoProcessor <NSObject>

- (std::shared_ptr<xhey::camera::Processor<std::shared_ptr<xhey::camera::VideoFrame>>>)cppProcessor;

@end

NS_ASSUME_NONNULL_END
