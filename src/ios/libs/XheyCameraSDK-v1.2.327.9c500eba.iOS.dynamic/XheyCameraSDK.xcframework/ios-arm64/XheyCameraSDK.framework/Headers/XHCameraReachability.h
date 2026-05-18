//
//  XHCameraReachability.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/24.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraReachability : NSObject

+ (instancetype)sharedInstance;

- (BOOL)isReachable;

- (void)addReachabilityCallback:(void (^)(BOOL reachable))callback;

- (void)removeReachabilityCallback:(void (^)(BOOL reachable))callback;

@end

NS_ASSUME_NONNULL_END
