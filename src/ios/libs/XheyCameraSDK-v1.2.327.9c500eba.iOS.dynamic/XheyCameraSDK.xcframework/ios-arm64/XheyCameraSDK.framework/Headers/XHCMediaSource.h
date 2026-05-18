//
//  XHCMediaSource.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/7/4.
//

#import "XHCMediaSink.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

extern NSString *const kXHCMediaConnectionNameVideo;
extern NSString *const kXHCMediaConnectionNameAudio;

@interface XHCMediaSource : NSObject

- (void)connectToSink:(id<XHCMediaSink>)sink withConnectionName:(NSString *)connectionName;

- (void)disconnectFromSink:(id<XHCMediaSink>)sink withConnectionName:(NSString *)connectionName;

- (void)publishMediaFrame:(id)frame withConnectionName:(NSString *)connectionName;

@end

NS_ASSUME_NONNULL_END
