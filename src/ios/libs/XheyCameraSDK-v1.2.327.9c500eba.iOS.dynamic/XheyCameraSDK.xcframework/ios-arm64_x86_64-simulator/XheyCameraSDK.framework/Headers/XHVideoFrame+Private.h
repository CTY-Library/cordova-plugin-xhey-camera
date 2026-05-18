//
//  XHVideoFrame.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHVideoFrame.h"
#include "common/media_types.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHVideoFrameAttributes (Private)

- (xhey::camera::VideoFrame::Attributes)toCppAttributes;

@end

@interface XHVideoFrame (Private)

- (std::shared_ptr<xhey::camera::VideoFrame>)toCppVideoFrame;

- (instancetype)initWithCppVideoFrame:(std::shared_ptr<xhey::camera::VideoFrame>)cppFrame;

@end

NS_ASSUME_NONNULL_END
