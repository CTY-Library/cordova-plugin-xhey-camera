//
//  XHAudioFrame.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/11/3.
//

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, XHAudioFrameType) {
    XHAudioFrameTypeUnknown,
    XHAudioFrameTypePCM,
    XHAudioFrameTypeAAC,
    XHAudioFrameTypeOpus,
};

typedef NS_ENUM(NSUInteger, XHAudioSampleFormatType) {
    XHAudioSampleFormatTypeUnknown,
    XHAudioSampleFormatTypeU8,
    XHAudioSampleFormatTypeS16LE,
    XHAudioSampleFormatTypeS24LE,
    XHAudioSampleFormatTypeS32LE,
    XHAudioSampleFormatTypeF32LE,
};

typedef NS_ENUM(NSUInteger, XHAudioChannelLayout) {
    XHAudioChannelLayoutUnknown,
    XHAudioChannelLayoutMono,
    XHAudioChannelLayoutStereo,
};

#define kXHCAudioFormat_Integer int16_t
#define kXHCAudioStreamDescription_SampleRate 44100
#define kXHCAudioStreamDescription_FormatID kAudioFormatLinearPCM
#define kXHCAudioStreamDescription_FormatFlags (kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked)
#define kXHCAudioStreamDescription_ChannelsPerFrame 1
#define kXHCAudioStreamDescription_FramesPerPacket 1
#define kXHCAudioStreamDescription_BitsPerChannel (8 * sizeof(kXHCAudioFormat_Integer))
#define kXHCAudioStreamDescription_BytesPerFrame (sizeof(kXHCAudioFormat_Integer))
#define kXHCAudioStreamDescription_BytesPerPacket (sizeof(kXHCAudioFormat_Integer))

@interface XHAudioFrame : NSObject

@property(nonatomic, assign) XHAudioFrameType type;

@property(nonatomic, strong) NSData *bufferData;

// 单位微妙
@property(nonatomic, assign) int64_t timestamp;

@property(nonatomic, assign) int32_t sampleRate;

@property(nonatomic, assign) int32_t channelNum;

@property(nonatomic, assign) XHAudioSampleFormatType sampleFormat;

@property(nonatomic, assign) XHAudioChannelLayout channelLayout;

@property(nonatomic, strong) id opaque;

@property(nonatomic, assign, readonly) CMSampleBufferRef sampleBuffer;

+ (XHAudioFrame *)audioFrameFromSampleBuffer:(CMSampleBufferRef)sampleBuffer;

@end

NS_ASSUME_NONNULL_END
