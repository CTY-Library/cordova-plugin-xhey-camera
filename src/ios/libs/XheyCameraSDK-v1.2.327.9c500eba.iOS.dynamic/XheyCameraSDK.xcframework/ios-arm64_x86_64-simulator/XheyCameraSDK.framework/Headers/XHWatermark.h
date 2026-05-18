//
//  XHWatermark.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import "XHVideoFrame.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHWatermark : NSObject

@property(nonatomic, assign) CGRect rect;

@property(nonatomic, assign) NSInteger watermarkId;

@property(nonatomic, strong) XHVideoFrame *videoFrame;

@end

NS_ASSUME_NONNULL_END
