#import "XheyCamera.h"
#import <UIKit/UIKit.h>

#import "XHCameraViewController.h"
#import "XHCameraViewConfig.h"
#import "XHCapturedImage.h"
#import "XHUserComment.h"
#import "XHCameraJsBridge.h"

@interface XheyCamera () <XHCameraViewControllerDelegate>
{
    NSDictionary* savedConfig;
    NSString* pendingCallbackId;
    UIViewController* presentedVC;
    NSDictionary* pendingOptions;
}
@end

@implementation XheyCamera

- (void)configure:(CDVInvokedUrlCommand*)command {
    CDVPluginResult* result = nil;
    NSDictionary* cfg = [command.arguments firstObject];
    if (cfg && [cfg isKindOfClass:[NSDictionary class]]) {
        savedConfig = cfg;
        result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsString:@"configured"];
    } else {
        result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:@"no_config"];
    }
    [self.commandDelegate sendPluginResult:result callbackId:command.callbackId];
}

- (void)takePhoto:(CDVInvokedUrlCommand*)command {
    NSDictionary* options = [command.arguments firstObject];
    NSMutableDictionary* merged = [NSMutableDictionary new];
    if (savedConfig) [merged addEntriesFromDictionary:savedConfig];
    if (options && [options isKindOfClass:[NSDictionary class]]) [merged addEntriesFromDictionary:options];

    // Determine credentials: prefer runtime/merged config, then Cordova preferences, then Info.plist
    NSString* appidVal = nil;
    NSString* secretVal = nil;
    if (merged[@"appid"]) appidVal = [NSString stringWithFormat:@"%@", merged[@"appid"]];
    if (merged[@"secretKey"]) secretVal = [NSString stringWithFormat:@"%@", merged[@"secretKey"]];
    if ((appidVal == nil || [appidVal length] == 0) || (secretVal == nil || [secretVal length] == 0)) {
        @try {
            if ([self.commandDelegate respondsToSelector:@selector(settings)]) {
                NSDictionary* settings = [self.commandDelegate settings];
                // support multiple common preference keys
                if ((appidVal == nil || [appidVal length] == 0)) {
                    if (settings[@"APP_ID"]) appidVal = [NSString stringWithFormat:@"%@", settings[@"APP_ID"]];
                    else if (settings[@"APPID"]) appidVal = [NSString stringWithFormat:@"%@", settings[@"APPID"]];
                    else if (settings[@"appid"]) appidVal = [NSString stringWithFormat:@"%@", settings[@"appid"]];
                }
                if ((secretVal == nil || [secretVal length] == 0)) {
                    if (settings[@"SECRET_KEY"]) secretVal = [NSString stringWithFormat:@"%@", settings[@"SECRET_KEY"]];
                    else if (settings[@"SECRETKEY"]) secretVal = [NSString stringWithFormat:@"%@", settings[@"SECRETKEY"]];
                    else if (settings[@"secret_key"]) secretVal = [NSString stringWithFormat:@"%@", settings[@"secret_key"]];
                }
            }
        } @catch (NSException *ex) {
            // ignore
        }
        if ((appidVal == nil || [appidVal length] == 0)) {
            NSString* v = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"xhey_appid"];
            if (v && [v length] > 0) appidVal = v;
        }
        if ((secretVal == nil || [secretVal length] == 0)) {
            NSString* v2 = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"xhey_secret_key"];
            if (v2 && [v2 length] > 0) secretVal = v2;
        }
    }

    // Build XHCameraViewConfig
    XHCameraViewConfig* cfg = [[XHCameraViewConfig alloc] init];
    if (appidVal) cfg.appid = appidVal;
    if (secretVal) cfg.secretKey = secretVal;
    if (merged[@"needPhotoConfirm"]) cfg.needPhotoConfirm = [merged[@"needPhotoConfirm"] boolValue];
    if (merged[@"maxImageCount"]) cfg.maxImageCount = [merged[@"maxImageCount"] integerValue];
    if (merged[@"groupWatermarkId"]) cfg.groupWatermarkId = [NSString stringWithFormat:@"%@", merged[@"groupWatermarkId"]];
    if (merged[@"customInputItems"] && [merged[@"customInputItems"] isKindOfClass:[NSDictionary class]]) {
        cfg.customInputItems = merged[@"customInputItems"];
    }

    // Keep callback id to reply later
    pendingCallbackId = command.callbackId;
    pendingOptions = merged;

    dispatch_async(dispatch_get_main_queue(), ^{
        XHCameraViewController* vc = [[XHCameraViewController alloc] initWithConfig:cfg delegate:(id<XHCameraViewControllerDelegate>)self];
        presentedVC = vc;
        UIViewController* root = UIApplication.sharedApplication.keyWindow.rootViewController;
        while (root.presentedViewController) root = root.presentedViewController;
        [root presentViewController:vc animated:YES completion:nil];
    });
}

- (void)takeBurst:(CDVInvokedUrlCommand*)command { [self takePhoto:command]; }
- (void)startPreview:(CDVInvokedUrlCommand*)command { [self takePhoto:command]; }

- (void)stopPreview:(CDVInvokedUrlCommand*)command {
    // Try JS bridge then notification
    Class js = NSClassFromString(@"XHCameraJsBridge");
    if (js && [js instancesRespondToSelector:@selector(stopPreviewWithCompletion:)]) {
        // nothing to call statically — send notification fallback
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"XheyCloseCameraNotification" object:nil];
    CDVPluginResult* res = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsString:@"preview_stop_requested"];
    [self.commandDelegate sendPluginResult:res callbackId:command.callbackId];
}

- (void)switchCamera:(CDVInvokedUrlCommand*)command {
    NSString* pos = [command.arguments firstObject]; if (![pos isKindOfClass:[NSString class]]) pos = @"back";
    // Use notification to request switch
    [[NSNotificationCenter defaultCenter] postNotificationName:@"XheySwitchCameraNotification" object:@{@"cameraPosition": pos}];
    CDVPluginResult* res = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsString:@"switch_requested"];
    [self.commandDelegate sendPluginResult:res callbackId:command.callbackId];
}

#pragma mark - XHCameraViewControllerDelegate

- (void)cameraViewController:(XHCameraViewController *)cameraViewController didCaptureStillImages:(NSArray<XHCapturedImage *> *)capturedImages {
    // Build images array and meta
    NSMutableArray* images = [NSMutableArray new];
    NSMutableArray* fileUris = [NSMutableArray new];
    NSMutableArray* ucos = [NSMutableArray new];
    for (XHCapturedImage* ci in capturedImages) {
        if (ci.imageData) {
            // Also save captured image data to the user's Photo Library
            UIImage *__nullable imgForSave = [UIImage imageWithData:ci.imageData];
            if (imgForSave) {
                UIImageWriteToSavedPhotosAlbum(imgForSave, self, @selector(image:didFinishSavingWithError:contextInfo:), NULL);
            }
            NSString* returnType = @"base64";
            if (pendingOptions && [pendingOptions isKindOfClass:[NSDictionary class]] && pendingOptions[@"returnType"]) {
                returnType = [NSString stringWithFormat:@"%@", pendingOptions[@"returnType"]];
            }
            if ([returnType isEqualToString:@"file"]) {
                // write to tmp
                NSString* tmp = NSTemporaryDirectory();
                NSString* sub = [tmp stringByAppendingPathComponent:@"xhey_camera"];
                NSError* err = nil;
                if (![[NSFileManager defaultManager] fileExistsAtPath:sub]) {
                    [[NSFileManager defaultManager] createDirectoryAtPath:sub withIntermediateDirectories:YES attributes:nil error:&err];
                }
                NSString* fname = [NSString stringWithFormat:@"%lld_%lu.jpg", (long long)([[NSDate date] timeIntervalSince1970]*1000), (unsigned long)[images count]];
                NSString* path = [sub stringByAppendingPathComponent:fname];
                BOOL wrote = [ci.imageData writeToFile:path atomically:YES];
                if (wrote) {
                    NSString* uri = [@"file://" stringByAppendingString:path];
                    [fileUris addObject:uri];
                } else {
                    // fallback to base64 if write failed
                    NSString* b64 = [ci.imageData base64EncodedStringWithOptions:0];
                    [images addObject:b64];
                }
            } else {
                NSString* b64 = [ci.imageData base64EncodedStringWithOptions:0];
                [images addObject:b64];
            }
        }
        XHUserComment* uc = ci.userComment;
        if (uc) {
            NSMutableDictionary* d = [NSMutableDictionary new];
            d[@"captureTimestampMs"] = @(uc.captureTimestampMs);
            d[@"latitude"] = @(uc.latitude);
            d[@"longitude"] = @(uc.longitude);
            d[@"altitude"] = @(uc.altitude);
            if (uc.antiFakeCode) d[@"antiFakeCode"] = uc.antiFakeCode;
            if (uc.location) d[@"location"] = uc.location;
            d[@"frontCamera"] = @(uc.frontCamera);
            d[@"mockAddress"] = @(uc.mockAddress);
            if (uc.rawJson) d[@"rawJson"] = uc.rawJson;
            [ucos addObject:d];
        }
    }

    NSMutableDictionary* out = [NSMutableDictionary new];
    if (images.count > 0) out[@"images"] = images;
    if (fileUris.count > 0) out[@"fileUris"] = fileUris;
    if (ucos.count == 1) out[@"meta"] = @{ @"userCommentObject": ucos.firstObject };
    else if (ucos.count > 1) out[@"meta"] = @{ @"userCommentObject": ucos };

    if (pendingCallbackId) {
        CDVPluginResult* pr = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsDictionary:out];
        [self.commandDelegate sendPluginResult:pr callbackId:pendingCallbackId];
        pendingCallbackId = nil;
        pendingOptions = nil;
    }

    // dismiss
    dispatch_async(dispatch_get_main_queue(), ^{
        [cameraViewController dismissViewControllerAnimated:YES completion:nil];
        presentedVC = nil;
    });
}

- (void)cameraViewControllerDidCancel:(XHCameraViewController *)cameraViewController {
    if (pendingCallbackId) {
        CDVPluginResult* pr = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:@"CANCELED"];
        [self.commandDelegate sendPluginResult:pr callbackId:pendingCallbackId];
        pendingCallbackId = nil;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [cameraViewController dismissViewControllerAnimated:YES completion:nil];
        presentedVC = nil;
    });
}

- (void)cameraViewController:(XHCameraViewController *)cameraViewController didFailWithError:(NSError *)error {
    if (pendingCallbackId) {
        CDVPluginResult* pr = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:[error localizedDescription]];
        [self.commandDelegate sendPluginResult:pr callbackId:pendingCallbackId];
        pendingCallbackId = nil;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [cameraViewController dismissViewControllerAnimated:YES completion:nil];
        presentedVC = nil;
    });
}

- (void)image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo {
    if (error) {
        NSLog(@"XheyCamera: failed saving image to Photos: %@", error);
    } else {
        NSLog(@"XheyCamera: image saved to Photos");
    }
}

@end
