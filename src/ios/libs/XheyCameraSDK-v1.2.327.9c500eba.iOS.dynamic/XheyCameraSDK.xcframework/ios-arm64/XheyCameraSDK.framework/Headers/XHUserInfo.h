//
//  XHUserInfo.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/12/18.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHUserInfo : NSObject

+ (instancetype)sharedInstance;

- (void)setup;

@property(nonatomic, copy, readonly) NSString *deviceId;

@end

NS_ASSUME_NONNULL_END
