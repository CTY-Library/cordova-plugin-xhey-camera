//
//  XHCLifeCycle.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/7/8.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol XHCLifeCycleProtocol <NSObject>

- (void)onApplicationWillResignActive;

- (void)onApplicationDidBecomeActive;

@end

@interface XHCLifeCycle : NSObject

- (void)addObserver:(id<XHCLifeCycleProtocol>)observer;

- (void)removeObserver:(id<XHCLifeCycleProtocol>)observer;

@property(nonatomic, assign, readonly) BOOL isApplicationActive;

@end

NS_ASSUME_NONNULL_END
