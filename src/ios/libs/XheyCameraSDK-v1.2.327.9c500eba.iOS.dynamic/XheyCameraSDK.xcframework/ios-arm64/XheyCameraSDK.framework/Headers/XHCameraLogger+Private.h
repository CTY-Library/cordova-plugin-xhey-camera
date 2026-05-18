//
//  XHCameraLogger+Private.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/23.
//

#import "XHCameraLogger.h"

#ifdef __cplusplus
extern "C" {
#endif

void XHCameraLogInfo(NSString *format, ...);

void XHCameraLogError(NSString *format, ...);

void XHCameraLogWarn(NSString *format, ...);

#ifdef __cplusplus
}
#endif

#define XHCLogInfo(format, ...) XHCameraLogInfo(format, ##__VA_ARGS__)

#define XHCLogError(format, ...) XHCameraLogError(format, ##__VA_ARGS__)

#define XHCLogWarn(format, ...) XHCameraLogWarn(format, ##__VA_ARGS__)
