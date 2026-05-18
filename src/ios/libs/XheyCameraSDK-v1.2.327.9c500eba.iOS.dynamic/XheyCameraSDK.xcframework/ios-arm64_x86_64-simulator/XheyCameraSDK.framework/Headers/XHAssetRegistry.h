//
//  XHAssetRegistry.h (SDK)
//  XheyCameraSDK
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHAssetRecord : NSObject

@property (nonatomic, strong) NSURL *fileURL;
@property (nonatomic, copy) NSString *mime;

@end

@interface XHAssetRegistry : NSObject

@property (class, nonatomic, readonly) XHAssetRegistry *shared;

- (NSString *)registerFileURL:(NSURL *)fileURL mime:(NSString *)mime;
- (nullable XHAssetRecord *)recordForFid:(NSString *)fid;

@end

NS_ASSUME_NONNULL_END