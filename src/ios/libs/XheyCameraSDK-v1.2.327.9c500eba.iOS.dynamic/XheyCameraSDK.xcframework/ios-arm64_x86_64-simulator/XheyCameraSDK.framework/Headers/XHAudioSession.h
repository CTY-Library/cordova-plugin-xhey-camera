//
//  XHAudioSession.h
//  Pods-XHCameraDemo
//
//  Created by leizh007 on 2025/7/8.
//

#import <Foundation/Foundation.h>
#import "XHAudioFrame.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHAudioSession : NSObject

- (void)start;

- (void)stop;

@property (nonatomic, copy) void (^onAudioFrameAvailable)(XHAudioFrame *audioFrame);

@end

NS_ASSUME_NONNULL_END
