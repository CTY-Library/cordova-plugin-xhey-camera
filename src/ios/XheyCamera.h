#import <Cordova/CDV.h>

@interface XheyCamera : CDVPlugin

- (void)configure:(CDVInvokedUrlCommand*)command;
- (void)takePhoto:(CDVInvokedUrlCommand*)command;
- (void)takeBurst:(CDVInvokedUrlCommand*)command;
- (void)startPreview:(CDVInvokedUrlCommand*)command;
- (void)stopPreview:(CDVInvokedUrlCommand*)command;
- (void)switchCamera:(CDVInvokedUrlCommand*)command;

@end
