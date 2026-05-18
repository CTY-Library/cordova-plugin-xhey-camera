//
//  XHCameraWKWebView.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/23.
//

#import <WebKit/WebKit.h>
#import "XHCameraMiscUtils.h"
#import "XHCameraViewConfig.h"
#import "XHCameraJsBridge.h"

NS_ASSUME_NONNULL_BEGIN

@interface XHCameraWKWebView : WKWebView

- (instancetype)initWithConfig:(XHCameraViewConfig *)config
              safeAreaTopInset:(CGFloat)safeAreaTopInset
           safeAreaBottomInset:(CGFloat)safeAreaBottomInset;

- (void)dispose;

@property (nonatomic, strong) XHCameraJsBridge *jsBridge;

@end

NS_ASSUME_NONNULL_END
