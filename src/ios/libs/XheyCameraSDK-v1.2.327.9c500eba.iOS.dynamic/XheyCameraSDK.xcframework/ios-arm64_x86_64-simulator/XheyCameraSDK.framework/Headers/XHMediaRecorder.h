//
//  XHMediaRecorder.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XHVideoInfo : NSObject

@property(nonatomic, assign) uint32_t width;

@property(nonatomic, assign) uint32_t height;

@property(nonatomic, strong) UIImage *previewImage;

@property(nonatomic, assign) double duration;

@property(nonatomic, strong) NSString *filePath;

@end

@class XHMediaRecorder;

@protocol XHMediaRecorderDelegate <NSObject>

- (void)mediaRecorderDidStartRecording:(XHMediaRecorder *)mediaRecorder;

- (void)mediaRecorder:(XHMediaRecorder *)mediaRecorder didFinishRecordingWithVideoInfo:(XHVideoInfo *)videoInfo;

- (void)mediaRecorder:(XHMediaRecorder *)mediaRecorder didFailRecordingWithError:(NSError *)error;

- (void)mediaRecorder:(XHMediaRecorder *)mediaRecorder didUpdateRecordingProgress:(double)progress;

@end

@interface XHMediaRecorder : NSObject

- (void)startWithFilePath:(NSString *)filePath
                    width:(uint32_t)width
                   height:(uint32_t)height
                 delegate:(id<XHMediaRecorderDelegate>)delegate;

- (void)stop;

- (BOOL)isRecording;

@end

NS_ASSUME_NONNULL_END
