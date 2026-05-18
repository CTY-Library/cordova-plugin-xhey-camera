//
//  XHPixelBufferWrapper.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHPixelBufferWrapper.h"
#include <memory>

NS_ASSUME_NONNULL_BEGIN

@interface XHPixelBufferWrapper (Private)

- (std::shared_ptr<void>)toCppPixelBuffer;

@end

NS_ASSUME_NONNULL_END
