//
//  XHMediaKitDefs+Private.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHMediaKitDefs.h"
#include "defs.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHMediaKitConfig (Private)

- (xhey::camera::MediaKitConfig)toCppConfig;

@end

extern xhey::camera::ProcessorGroupType ConvertProcessorGroupType(ProcessorGroupType ocType);

NS_ASSUME_NONNULL_END
