//
//  XHZipCallbackRunner.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/12/19.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHZipCallbackRunner : NSObject

- (instancetype)initWithTotalTaskCount:(NSInteger)totalTaskCount
                             onAllDone:(dispatch_block_t)onAllDone;

- (void)markTaskDone;

@end

NS_ASSUME_NONNULL_END
