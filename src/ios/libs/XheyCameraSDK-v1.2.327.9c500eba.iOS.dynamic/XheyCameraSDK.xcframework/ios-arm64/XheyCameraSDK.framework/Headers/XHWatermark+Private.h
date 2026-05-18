//
//  XHWatermark.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHWatermark.h"
#include "common/media_types.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHWatermark (Private)

- (std::shared_ptr<xhey::camera::Watermark>)toCppWatermark;

@end

NS_ASSUME_NONNULL_END
