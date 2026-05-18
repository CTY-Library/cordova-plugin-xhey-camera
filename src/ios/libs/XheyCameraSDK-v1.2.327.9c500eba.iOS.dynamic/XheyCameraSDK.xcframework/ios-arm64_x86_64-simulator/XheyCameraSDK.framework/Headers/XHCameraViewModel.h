//
//  XHCameraViewModel.h
//  Pods
//
//  Created by leizh007 on 2025/3/26.
//

#import <Foundation/Foundation.h>
#import "XHCameraConfig.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraViewModel : NSObject

@property (nonatomic, assign) NSInteger lastCameraPageDuration;

@property (nonatomic, assign) BOOL lastCameraPageDurationReported;

@property (nonatomic, strong) XHCameraConfig *cameraConfig;

- (void)onCameraPageShow;

- (void)onCameraPageDismiss;

- (void)saveCameraConfig;

@end

NS_ASSUME_NONNULL_END
