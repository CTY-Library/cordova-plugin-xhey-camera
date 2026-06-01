#import "XheyCamera.h"
#import <UIKit/UIKit.h>
#import <objc/message.h>

// Avoid compile-time dependency on XheyCameraSDK headers — use runtime lookup.
@class XHCameraViewController;
@class XHCameraViewConfig;

@interface XheyCamera ()
{
    NSDictionary* savedConfig;
    NSString* pendingCallbackId;
    UIViewController* presentedVC;
    NSDictionary* pendingOptions;
    NSString* _mappid;
    NSString* _msecretKey;
}
@end

@implementation XheyCamera

- (void)pluginInitialize {
    [super pluginInitialize];
    NSLog(@"XheyCamera: plugin initialized");
    
    // Try multiple ways to get settings
    NSDictionary* settings1 = self.commandDelegate.settings;
    NSDictionary* settings2 = nil;
    if ([self.viewController respondsToSelector:@selector(settings)]) {
        settings2 = [self.viewController settings];
    }
    
    NSLog(@"XheyCamera: commandDelegate.settings = %@", settings1);
    NSLog(@"XheyCamera: viewController.settings = %@", settings2);
    
    // Try to get APPID and SECRET_KEY from both sources with multiple key formats
    _mappid = [settings1 objectForKey:@"APPID"];
    if (!_mappid || [_mappid length] == 0) {
        _mappid = [settings2 objectForKey:@"APPID"];
    }
    if (!_mappid || [_mappid length] == 0) {
        _mappid = [settings1 objectForKey:@"appid"];
    }
    if (!_mappid || [_mappid length] == 0) {
        _mappid = [settings2 objectForKey:@"appid"];
    }
    if (!_mappid || [_mappid length] == 0) {
        _mappid = [settings1 objectForKey:@"APP_ID"];
    }
    if (!_mappid || [_mappid length] == 0) {
        _mappid = [settings2 objectForKey:@"APP_ID"];
    }
    
    _msecretKey = [settings1 objectForKey:@"SECRET_KEY"];
    if (!_msecretKey || [_msecretKey length] == 0) {
        _msecretKey = [settings2 objectForKey:@"SECRET_KEY"];
    }
    if (!_msecretKey || [_msecretKey length] == 0) {
        _msecretKey = [settings1 objectForKey:@"secretKey"];
    }
    if (!_msecretKey || [_msecretKey length] == 0) {
        _msecretKey = [settings2 objectForKey:@"secretKey"];
    }
    if (!_msecretKey || [_msecretKey length] == 0) {
        _msecretKey = [settings1 objectForKey:@"SECRETKEY"];
    }
    if (!_msecretKey || [_msecretKey length] == 0) {
        _msecretKey = [settings2 objectForKey:@"SECRETKEY"];
    }
    
    // If still not found, try from Info.plist
    if (!_mappid || [_mappid length] == 0) {
        _mappid = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"xhey_appid"];
        if (_mappid) {
            NSLog(@"XheyCamera: found appid in Info.plist = %@", _mappid);
        }
    }
    if (!_msecretKey || [_msecretKey length] == 0) {
        _msecretKey = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"xhey_secret_key"];
        if (_msecretKey) {
            NSLog(@"XheyCamera: found secretKey in Info.plist = %@", _msecretKey);
        }
    }
    
    NSLog(@"XheyCamera: loaded from settings - APPID='%@', SECRET_KEY='%@'", _mappid, _msecretKey);
}

- (void)configure:(CDVInvokedUrlCommand*)command {
    NSDictionary* cfg = [command.arguments firstObject];
    CDVPluginResult* result = nil;
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

    // Debug log to check merged config
    NSLog(@"XheyCamera: merged config = %@", merged);

    // Resolve credentials from merged config, Cordova settings, or Info.plist
    NSString* appidVal = [self _stringForKey:@"appid" inDict:merged];
    NSString* secretVal = [self _stringForKey:@"secretKey" inDict:merged];
    
    // Debug log to check initial values
    NSLog(@"XheyCamera: initial appid='%@', secretKey='%@'", appidVal, secretVal);

    // Use values loaded from pluginInitialize if not provided in merged config
    if ((appidVal == nil || [appidVal length] == 0) && _mappid && [_mappid length] > 0) {
        appidVal = _mappid;
        NSLog(@"XheyCamera: using appid from pluginInitialize = %@", appidVal);
    }
    if ((secretVal == nil || [secretVal length] == 0) && _msecretKey && [_msecretKey length] > 0) {
        secretVal = _msecretKey;
        NSLog(@"XheyCamera: using secretKey from pluginInitialize = %@", secretVal);
    }

    // Log final values before proceeding
    NSLog(@"XheyCamera: final appid='%@', secretKey='%@'", appidVal, secretVal);

    // Check if credentials are available before proceeding
    if (!appidVal || [appidVal length] == 0 || !secretVal || [secretVal length] == 0) {
        // Report error immediately if credentials are missing
        NSString *errorMsg = [NSString stringWithFormat:@"Missing required credentials: appid='%@', secretKey='%@'", 
                              appidVal ? appidVal : @"MISSING", 
                              secretVal ? secretVal : @"MISSING"];
        NSLog(@"XheyCamera: %@", errorMsg);
        
        CDVPluginResult* result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:errorMsg];
        [self.commandDelegate sendPluginResult:result callbackId:command.callbackId];
        return;
    }

    // Build SDK config object dynamically
    id cfg = nil;
    Class cfgClass = NSClassFromString(@"XHCameraViewConfig");
    if (cfgClass) cfg = [[cfgClass alloc] init];
    if (cfg) {
        @try { 
            if (appidVal && [appidVal isKindOfClass:[NSString class]] && [appidVal length] > 0) {
                [cfg setValue:appidVal forKey:@"appid"]; 
                NSLog(@"XheyCamera: successfully set appid in config");
            } 
        } @catch (NSException *e) {}
        @try { 
            if (secretVal && [secretVal isKindOfClass:[NSString class]] && [secretVal length] > 0) {
                [cfg setValue:secretVal forKey:@"secretKey"]; 
                NSLog(@"XheyCamera: successfully set secretKey in config");
            } 
        } @catch (NSException *e) {}
        @try { 
            if (merged[@"needPhotoConfirm"]) {
                [cfg setValue:merged[@"needPhotoConfirm"] forKey:@"needPhotoConfirm"]; 
            } 
        } @catch (NSException *e) {}
        @try { 
            if (merged[@"maxImageCount"]) {
                [cfg setValue:merged[@"maxImageCount"] forKey:@"maxImageCount"]; 
            } 
        } @catch (NSException *e) {}
        @try { 
            if (merged[@"groupWatermarkId"] && [merged[@"groupWatermarkId"] isKindOfClass:[NSString class]]) {
                [cfg setValue:[NSString stringWithFormat:@"%@", merged[@"groupWatermarkId"]] forKey:@"groupWatermarkId"]; 
            } 
        } @catch (NSException *e) {}
        @try { 
            if (merged[@"customInputItems"] && [merged[@"customInputItems"] isKindOfClass:[NSDictionary class]]) {
                [cfg setValue:merged[@"customInputItems"] forKey:@"customInputItems"]; 
            } 
        } @catch (NSException *e) {}
        @try { 
            if (merged[@"useCustomUI"]) {
                [cfg setValue:merged[@"useCustomUI"] forKey:@"useCustomUI"]; 
            } 
        } @catch (NSException *e) {}
        @try { 
            BOOL useCustomUI = NO;
            if (merged[@"useCustomUI"]) {
                useCustomUI = [merged[@"useCustomUI"] boolValue];
            }
            
            NSString* resourceDir = nil;
            if (useCustomUI) {
                // Use custom UI path when useCustomUI is true
                resourceDir = merged[@"resourceDir"];
                if (!resourceDir || [resourceDir length] == 0) {
                    resourceDir = [self resolveResourceDir];
                }
                NSLog(@"XheyCamera: using custom UI resourceDir = %@", resourceDir);
            } else {
                // Use default official path when useCustomUI is false
                resourceDir = @"";
                NSLog(@"XheyCamera: using default official resourceDir");
            }
            
            if (resourceDir && [resourceDir isKindOfClass:[NSString class]]) {
                [cfg setValue:resourceDir forKey:@"resourceDir"];
                NSLog(@"XheyCamera: resourceDir = %@", resourceDir);
            } 
        } @catch (NSException *e) {}
    }

    pendingCallbackId = command.callbackId;
    pendingOptions = merged;

    dispatch_async(dispatch_get_main_queue(), ^{
        id vc = nil;
        Class vcClass = NSClassFromString(@"XHCameraViewController");
        SEL initSel = NSSelectorFromString(@"initWithConfig:delegate:");
        if (vcClass) {
            if ([vcClass instancesRespondToSelector:initSel]) {
                id alloced = [vcClass alloc];
                // Only proceed if both appid and secretKey are confirmed to be non-nil
                if(cfg && appidVal && secretVal && [appidVal length] > 0 && [secretVal length] > 0) {
                    NSLog(@"XheyCamera: initializing XHCameraViewController with config");
                    vc = ((id (*)(id, SEL, id, id))objc_msgSend)(alloced, initSel, cfg, (id)self);
                } else {
                    // This shouldn't happen given our checks above, but just in case
                    if (pendingCallbackId) {
                        CDVPluginResult* pr = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:@"internal_error_missing_credentials"];
                        [self.commandDelegate sendPluginResult:pr callbackId:pendingCallbackId];
                        pendingCallbackId = nil;
                        pendingOptions = nil;
                    }
                    return;
                }
            } else if ([vcClass instancesRespondToSelector:@selector(init)]) {
                vc = [[vcClass alloc] init];
            }
        }

        presentedVC = vc;
        UIViewController* root = [UIApplication sharedApplication].keyWindow.rootViewController;
        while (root.presentedViewController) root = root.presentedViewController;
        if (vc && [root respondsToSelector:@selector(presentViewController:animated:completion:)]) {
            [root presentViewController:vc animated:YES completion:nil];
        } else {
            // SDK not available
            if (pendingCallbackId) {
                CDVPluginResult* pr = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:@"sdk_not_available"];
                [self.commandDelegate sendPluginResult:pr callbackId:pendingCallbackId];
                pendingCallbackId = nil;
                pendingOptions = nil;
            }
        }
    });
}

- (void)takeBurst:(CDVInvokedUrlCommand*)command { [self takePhoto:command]; }
- (void)startPreview:(CDVInvokedUrlCommand*)command { [self takePhoto:command]; }

- (void)stopPreview:(CDVInvokedUrlCommand*)command {
    Class js = NSClassFromString(@"XHCameraJsBridge");
    if (js && [js instancesRespondToSelector:@selector(stopPreviewWithCompletion:)]) {
        // prefer runtime bridge
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"XheyCloseCameraNotification" object:nil];
    CDVPluginResult* res = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsString:@"preview_stop_requested"];
    [self.commandDelegate sendPluginResult:res callbackId:command.callbackId];
}

- (void)switchCamera:(CDVInvokedUrlCommand*)command {
    NSString* pos = [command.arguments firstObject]; if (![pos isKindOfClass:[NSString class]]) pos = @"back";
    [[NSNotificationCenter defaultCenter] postNotificationName:@"XheySwitchCameraNotification" object:@{@"cameraPosition": pos}];
    CDVPluginResult* res = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsString:@"switch_requested"];
    [self.commandDelegate sendPluginResult:res callbackId:command.callbackId];
}

#pragma mark - Delegate callbacks (runtime-friendly)

- (void)cameraViewController:(id)cameraViewController didCaptureStillImages:(NSArray *)capturedImages {
    NSMutableArray* images = [NSMutableArray new];
    NSMutableArray* fileUris = [NSMutableArray new];
    NSMutableArray* ucos = [NSMutableArray new];

    for (id ci in capturedImages) {
        NSData *imgData = nil;
        @try { imgData = [ci valueForKey:@"imageData"]; } @catch (NSException *e) { imgData = nil; }
        if (imgData) {
            UIImage *imgForSave = [UIImage imageWithData:imgData];
            if (imgForSave) UIImageWriteToSavedPhotosAlbum(imgForSave, self, @selector(image:didFinishSavingWithError:contextInfo:), NULL);

            NSString* returnType = @"base64";
            if (pendingOptions && [pendingOptions isKindOfClass:[NSDictionary class]] && pendingOptions[@"returnType"]) {
                returnType = [NSString stringWithFormat:@"%@", pendingOptions[@"returnType"]];
            }
            if ([returnType isEqualToString:@"file"]) {
                NSString* tmp = NSTemporaryDirectory();
                NSString* sub = [tmp stringByAppendingPathComponent:@"xhey_camera"];
                NSError* err = nil;
                if (![[NSFileManager defaultManager] fileExistsAtPath:sub]) {
                    [[NSFileManager defaultManager] createDirectoryAtPath:sub withIntermediateDirectories:YES attributes:nil error:&err];
                }
                NSString* fname = [NSString stringWithFormat:@"%lld_%lu.jpg", (long long)([[NSDate date] timeIntervalSince1970]*1000), (unsigned long)[images count]];
                NSString* path = [sub stringByAppendingPathComponent:fname];
                BOOL wrote = NO;
                @try { wrote = [imgData writeToFile:path atomically:YES]; } @catch (NSException *e) { wrote = NO; }
                if (wrote) {
                    NSString* uri = [@"file://" stringByAppendingString:path];
                    [fileUris addObject:uri];
                } else {
                    NSString* b64 = [((NSData*)imgData) base64EncodedStringWithOptions:0];
                    [images addObject:b64];
                }
            } else {
                NSString* b64 = [((NSData*)imgData) base64EncodedStringWithOptions:0];
                [images addObject:b64];
            }
        }

        id uc = nil;
        @try { uc = [ci valueForKey:@"userComment"]; } @catch (NSException *e) { uc = nil; }
        if (uc) {
            NSMutableDictionary* d = [NSMutableDictionary new];
            @try { id ts = [uc valueForKey:@"captureTimestampMs"]; if (ts) d[@"captureTimestampMs"] = ts; } @catch (NSException *e) {}
            @try { id lat = [uc valueForKey:@"latitude"]; if (lat) d[@"latitude"] = lat; } @catch (NSException *e) {}
            @try { id lng = [uc valueForKey:@"longitude"]; if (lng) d[@"longitude"] = lng; } @catch (NSException *e) {}
            @try { id alt = [uc valueForKey:@"altitude"]; if (alt) d[@"altitude"] = alt; } @catch (NSException *e) {}
            @try { id af = [uc valueForKey:@"antiFakeCode"]; if (af) d[@"antiFakeCode"] = af; } @catch (NSException *e) {}
            @try { id loc = [uc valueForKey:@"location"]; if (loc) d[@"location"] = loc; } @catch (NSException *e) {}
            @try { id front = [uc valueForKey:@"frontCamera"]; if (front) d[@"frontCamera"] = front; } @catch (NSException *e) {}
            @try { id mock = [uc valueForKey:@"mockAddress"]; if (mock) d[@"mockAddress"] = mock; } @catch (NSException *e) {}
            @try { id raw = [uc valueForKey:@"rawJson"]; if (raw) d[@"rawJson"] = raw; } @catch (NSException *e) {}
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

    dispatch_async(dispatch_get_main_queue(), ^{
        if ([cameraViewController respondsToSelector:@selector(dismissViewControllerAnimated:completion:)]) {
            [cameraViewController dismissViewControllerAnimated:YES completion:nil];
        }
        presentedVC = nil;
    });
}

- (void)cameraViewControllerDidCancel:(id)cameraViewController {
    if (pendingCallbackId) {
        CDVPluginResult* pr = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:@"CANCELED"];
        [self.commandDelegate sendPluginResult:pr callbackId:pendingCallbackId];
        pendingCallbackId = nil;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([cameraViewController respondsToSelector:@selector(dismissViewControllerAnimated:completion:)]) {
            [cameraViewController dismissViewControllerAnimated:YES completion:nil];
        }
        presentedVC = nil;
    });
}

- (void)cameraViewController:(id)cameraViewController didFailWithError:(NSError *)error {
    if (pendingCallbackId) {
        CDVPluginResult* pr = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:[error localizedDescription]];
        [self.commandDelegate sendPluginResult:pr callbackId:pendingCallbackId];
        pendingCallbackId = nil;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([cameraViewController respondsToSelector:@selector(dismissViewControllerAnimated:completion:)]) {
            [cameraViewController dismissViewControllerAnimated:YES completion:nil];
        }
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

#pragma mark - Helpers

- (NSString*)_stringForKey:(NSString*)key inDict:(NSDictionary*)dict {
    id v = dict[key];
    if (!v) return nil;
    if ([v isKindOfClass:[NSString class]]) return v;
    return [NSString stringWithFormat:@"%@", v];
}

- (NSString*)_firstPresentStringForKeys:(NSArray*)keys inDict:(NSDictionary*)dict {
    if (!dict) return nil;
    for (id k in keys) {
        id v = dict[k];
        if (v) return [NSString stringWithFormat:@"%@", v];
    }
    return nil;
}

- (NSString*)resolveResourceDir {
    NSBundle* bundle = [NSBundle mainBundle];
    
    NSLog(@"XheyCamera: resolveResourceDir: checking custom UI locations");
    
    // Priority 1: Custom UI in MyXheyVue directory
    NSString* myXheyVuePath = [bundle pathForResource:@"index.html" ofType:nil inDirectory:@"XheyCameraSDKAssets/MyXheyVue"];
    if (myXheyVuePath) {
        NSString* dirPath = [myXheyVuePath stringByDeletingLastPathComponent];
        NSLog(@"XheyCamera: resolveResourceDir: found MyXheyVue -> %@", dirPath);
        return dirPath;
    }
    NSLog(@"XheyCamera: resolveResourceDir: MyXheyVue not found");
    
    // Priority 2: Custom UI in root XheyCameraSDKAssets directory
    NSString* rootIndexPath = [bundle pathForResource:@"index.html" ofType:nil inDirectory:@"XheyCameraSDKAssets"];
    if (rootIndexPath) {
        NSString* dirPath = [rootIndexPath stringByDeletingLastPathComponent];
        NSLog(@"XheyCamera: resolveResourceDir: found root XheyCameraSDKAssets -> %@", dirPath);
        return dirPath;
    }
    NSLog(@"XheyCamera: resolveResourceDir: root XheyCameraSDKAssets not found");
    
    // Priority 3: Default SDK bundle path
    NSString* bundlePath = [bundle pathForResource:@"XheyCameraSDKResource" ofType:@"bundle"];
    if (bundlePath) {
        NSLog(@"XheyCamera: resolveResourceDir: using default SDK bundle -> %@", bundlePath);
        return bundlePath;
    }
    
    // Fallback: empty string (SDK will use internal defaults)
    NSLog(@"XheyCamera: resolveResourceDir: no custom UI found, using SDK defaults");
    return @"";
}
@end