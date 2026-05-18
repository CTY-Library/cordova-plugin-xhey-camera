//
//  XHAssetSchemeHandler.h (SDK)
//  XheyCameraSDK
//

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

NS_ASSUME_NONNULL_BEGIN

// 处理 xhasset://fid 自定义资源加载，用于 WKWebView 播放/展示本地拍摄的媒体文件
@interface XHAssetSchemeHandler : NSObject <WKURLSchemeHandler>

@end

NS_ASSUME_NONNULL_END