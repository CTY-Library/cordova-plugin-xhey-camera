//
//  XHCameraMotionManager.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/23.
//

#import <Foundation/Foundation.h>
#import "XHCameraMiscUtils.h"

NS_ASSUME_NONNULL_BEGIN

@class XHCameraMotionManager;

@protocol XHCameraMotionManagerDelegate <NSObject>

- (void)motionManager:(XHCameraMotionManager *)motionManager
 didUpdateOrientation:(XHCameraDeviceOrientation)orientation;

- (UIViewController *)workingController;

@end

@interface XHCameraMotionManager : NSObject

- (void)start;

- (void)stop;

- (NSInteger)rotationValue;

- (UIInterfaceOrientation)currentInterfaceOrientation;

- (void)wokingControllerInterfaceOrientationDidChanged:(XHCameraDeviceOrientation)oirentation;

@property (nonatomic, assign, readonly) XHCameraDeviceOrientation orientation;

@property (nonatomic, weak) id<XHCameraMotionManagerDelegate> delegate;

@end

NS_ASSUME_NONNULL_END
