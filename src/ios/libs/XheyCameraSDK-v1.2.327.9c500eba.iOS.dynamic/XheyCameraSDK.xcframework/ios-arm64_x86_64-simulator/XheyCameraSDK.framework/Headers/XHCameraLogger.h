//
//  XHCameraLogger.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/23.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol XHCameraLoggerProtocol <NSObject>

- (void)logInfo:(NSString *)info;

- (void)logError:(NSString *)error;

- (void)logWarn:(NSString *)warn;

@end

@interface XHCameraLogger : NSObject

+ (void)registerLogger:(id<XHCameraLoggerProtocol>)logger;

@end

NS_ASSUME_NONNULL_END
