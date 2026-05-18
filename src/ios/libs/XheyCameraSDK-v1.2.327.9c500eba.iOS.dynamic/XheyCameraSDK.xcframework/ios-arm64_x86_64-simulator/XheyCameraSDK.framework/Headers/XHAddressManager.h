//
//  XHAddressManager.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/12/18.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^XHAddressManagerCallback)(NSString *_Nullable address, NSError *_Nullable error);

@interface XHAddressManager : NSObject

+ (void)requestAddressWithLatitude:(double)latitude longitude:(double)longitude completion:(XHAddressManagerCallback)callback;

+ (void)cancel;

+ (NSString *)getCurrentAddress;

@end

NS_ASSUME_NONNULL_END
