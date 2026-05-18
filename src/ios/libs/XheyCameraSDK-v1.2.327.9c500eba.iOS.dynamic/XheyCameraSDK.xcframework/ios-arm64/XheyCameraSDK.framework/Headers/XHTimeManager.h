//
//  XHTimeManager.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/12/17.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^XHTimeManagerCallback)(int64_t timestamp, BOOL inChina);

@interface XHTimeManager : NSObject

+ (void)refreshWithCallback:(XHTimeManagerCallback)callback;

+ (void)cancel;

+ (int64_t)currentTime;

+ (BOOL)isInChina;

@end

NS_ASSUME_NONNULL_END
