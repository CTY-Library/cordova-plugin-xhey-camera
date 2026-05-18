//
//  XHCapturedImage.h
//  XheyCameraSDK
//

#import "XHUserComment.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * 拍摄结果，包含 JPEG 图片数据和对应的 UserComment 元数据。
 */
@interface XHCapturedImage : NSObject

/** JPEG 压缩后的图片数据。如需 UIImage，可通过 [UIImage imageWithData:imageData] 获取。 */
@property(nonatomic, strong, readonly) NSData *imageData;

/** 拍照时的元数据，包含时间、位置、防伪码等 */
@property(nonatomic, strong, readonly) XHUserComment *userComment;

- (instancetype)initWithImageData:(NSData *)imageData userComment:(XHUserComment *)userComment;

@end

NS_ASSUME_NONNULL_END
