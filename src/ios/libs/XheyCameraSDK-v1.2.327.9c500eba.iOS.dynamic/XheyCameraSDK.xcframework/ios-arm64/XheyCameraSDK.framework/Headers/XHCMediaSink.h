//
//  XHCMediaSink.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/7/4.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol XHCMediaSink <NSObject>

- (void)onMediaFrame:(id)frame withConnectionName:(NSString *)connectionName;

@end

NS_ASSUME_NONNULL_END
