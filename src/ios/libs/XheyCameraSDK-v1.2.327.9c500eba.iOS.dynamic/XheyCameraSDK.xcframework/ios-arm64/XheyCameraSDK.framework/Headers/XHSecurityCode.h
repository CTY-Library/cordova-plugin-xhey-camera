//
//  XHSecurityCode.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/12/19.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHSecurityCode : NSObject

+ (NSString *)generateSecurityCodeWithTimestamp:(int64_t)timestamp latitude:(double)latitude longitude:(double)longitude;

@end

NS_ASSUME_NONNULL_END
