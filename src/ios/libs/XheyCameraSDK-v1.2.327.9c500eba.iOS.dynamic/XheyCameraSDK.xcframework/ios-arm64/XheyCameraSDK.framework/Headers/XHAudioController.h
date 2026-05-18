//
//  XHAudioController.h
//  Pods-XHCameraDemo
//
//  Created by leizh007 on 2025/7/8.
//

#import <Foundation/Foundation.h>
#import "XHCMediaSource.h"
#import "XHAudioFrame.h"
#import "XHCLifeCycle.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHAudioController : XHCMediaSource

+ (void)checkAudioPermissionWithCompletion:(void (^)(BOOL granted))completion;

+ (BOOL)hasAudioPermission;

- (instancetype)initWithLifeCycle:(XHCLifeCycle *)lifeCycle;

- (void)start;

- (void)stop;

@end

NS_ASSUME_NONNULL_END
