//
//  XHUserComment.h
//  XheyCameraSDK
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * 照片拍摄时的用户信息，包含拍照时间、位置、防伪码等元数据。
 * 从 H5 端传回的 UserComment JSON 反序列化而来。
 */
@interface XHUserComment : NSObject

/** 拍照时间戳（毫秒） */
@property(nonatomic, assign, readonly) long long captureTimestampMs;

/** 纬度 */
@property(nonatomic, assign, readonly) double latitude;

/** 经度 */
@property(nonatomic, assign, readonly) double longitude;

/** 海拔 */
@property(nonatomic, assign, readonly) double altitude;

/** 防伪码 */
@property(nonatomic, copy, readonly) NSString *antiFakeCode;

/** 拍照位置描述 */
@property(nonatomic, copy, readonly) NSString *location;

/** 是否前置摄像头 */
@property(nonatomic, assign, readonly) BOOL frontCamera;

/** 是否模拟定位 */
@property(nonatomic, assign, readonly) BOOL mockAddress;

/** 原始 JSON 字符串，可用于自定义解析 */
@property(nonatomic, copy, readonly) NSString *rawJson;

- (instancetype)initWithJsonString:(NSString *)jsonString;

@end

NS_ASSUME_NONNULL_END
