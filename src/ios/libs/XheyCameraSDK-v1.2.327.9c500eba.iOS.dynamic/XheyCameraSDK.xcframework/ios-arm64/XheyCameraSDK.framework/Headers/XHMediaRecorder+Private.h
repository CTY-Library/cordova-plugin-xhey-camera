//
//  XHMediaRecorder+Private.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHMediaRecorder.h"
#include "recorder/media_recorder.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHMediaRecorder (Private)

- (instancetype)initWithCppMediaRecorder:(const std::shared_ptr<xhey::camera::MediaRecorder> &)mediaRecorder;

@end

NS_ASSUME_NONNULL_END
