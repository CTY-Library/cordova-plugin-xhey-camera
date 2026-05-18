#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHSignatureManager : NSObject

+ (void)setup;

/// Generate HMAC-SHA256 signature for the given data string, returns base64-encoded signature.
+ (NSString *)generateSignature:(NSString *)data;

@end

NS_ASSUME_NONNULL_END
