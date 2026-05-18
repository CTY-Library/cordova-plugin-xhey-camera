//
//  XHMediaKitUtils.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import <Foundation/Foundation.h>
#include <functional>

inline std::function<void()> ConvertBlockToStdFunction(dispatch_block_t block) {
    if (!block)
        return nullptr;

    dispatch_block_t copiedBlock = [block copy];

    return [copiedBlock]() {
        @autoreleasepool {
            copiedBlock();
        }
    };
}
