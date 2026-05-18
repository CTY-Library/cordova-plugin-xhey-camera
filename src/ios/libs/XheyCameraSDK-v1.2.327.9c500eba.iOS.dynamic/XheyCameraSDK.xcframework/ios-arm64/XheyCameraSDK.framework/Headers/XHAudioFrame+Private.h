//
//  XHAudioFrame.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHAudioFrame.h"
#include "common/media_types.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHAudioFrame (Private)

- (std::shared_ptr<xhey::camera::AudioFrame>)toCppAudioFrame;

- (instancetype)initWithCppAudioFrame:(const std::shared_ptr<xhey::camera::AudioFrame> &)audio_frame;

@end

NS_ASSUME_NONNULL_END
