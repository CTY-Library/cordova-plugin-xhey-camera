//
//  XHMediaKit.h
//  Pods
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHAudioProcessor.h"
#import "XHCMediaSink.h"
#import "XHMediaKitDefs.h"
#import "XHMediaRecorder.h"
#import "XHVideoProcessor.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHMediaKit : NSObject <XHCMediaSink>

@property(nonatomic, readonly, strong) XHMediaRecorder *mediaRecorder;

- (instancetype)initWithConfig:(XHMediaKitConfig *)config;

- (void)addVideoProcessor:(id<XHVideoProcessor>)videoProcessor toGroup:(ProcessorGroupType)group;

- (void)removeVideoProcessor:(id<XHVideoProcessor>)videoProcessor fromGroup:(ProcessorGroupType)group;

- (void)addAudioProcessor:(id<XHAudioProcessor>)audioProcessor toGroup:(ProcessorGroupType)group;

- (void)removeAudioProcessor:(id<XHAudioProcessor>)audioProcessor fromGroup:(ProcessorGroupType)group;

- (void)flushWithCompletion:(dispatch_block_t)completion;

@end

NS_ASSUME_NONNULL_END
