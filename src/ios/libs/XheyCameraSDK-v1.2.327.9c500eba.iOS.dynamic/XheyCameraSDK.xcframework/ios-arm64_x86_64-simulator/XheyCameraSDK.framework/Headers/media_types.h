#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "gl/texture.h"
#include "truephoto_types.h"

namespace xhey::camera {

enum class ScaleMode : int32_t {
    kAspectFit,
    kAspectFill,
    kScaleToFill,
};

struct ViewPort {
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    ScaleMode scale_mode = ScaleMode::kAspectFit;

    ViewPort() = default;
    ViewPort(int32_t x, int32_t y, int32_t width, int32_t height, ScaleMode scale_mode)
        : x(x), y(y), width(width), height(height), scale_mode(scale_mode) {}
};

struct Rect {
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;

    Rect(double _x, double _y, double _width, double _height) : x(_x), y(_y), width(_width), height(_height) {}
    Rect() = default;
};

class VideoFrame;

struct Watermark {
public:
    Rect rect;
    std::shared_ptr<VideoFrame> video_frame;
    uint32_t id;

    enum WatermarkType {
        kWatermarkTypeOfficial = 0,
        kWatermarkTypeNormal = 1,
        kWatermarkTypeSecurityCode = 2,
    };

    enum OfficialWatermarkStyle {
        kOfficialWatermarkStyleOneLineYellowRealTimeMark = 10010,
        kOfficialWatermarkStyleBlueRealTimeMark = 10011,
    };

    // 第三个参数，否的话是用来显示的，包含防伪+防伪码，是的话是用来检测的，只会保护防伪码
    static Rect ComputeSecurityCodeWatermarkRect(float canvas_ratio, OfficialWatermarkStyle style, bool only_security_code);
};

class MediaData {
public:
    static MediaData *Create(uint8_t *data, size_t size) { return new MediaData(data, size); }

    static MediaData *CreateNoCopyWithDeallocator(uint8_t *data, size_t size, std::function<void(uint8_t *)> deallocator) {
        return new MediaData(data, size, deallocator);
    }

    static MediaData *CreateNoCopy(uint8_t *data, size_t size) {
        return new MediaData(data, size, [](uint8_t *ptr) { /* do nothing*/ });
    }

    static MediaData *CreateEmpty(size_t size) { return new MediaData(nullptr, size); }

    static MediaData *CreateEmptyPlane(int linesize, int height);

    MediaData(MediaData &&other) {
        buf_ = std::move(other.buf_);
        size_ = other.size_;
        other.buf_ = nullptr;
        other.size_ = 0;
    }

    inline size_t Size() const { return size_; }
    uint8_t *Data() const { return buf_.get(); }

    MediaData *Ref() const;

private:
    MediaData(uint8_t *data, size_t size, std::function<void(uint8_t *)> deallocator = nullptr);
    MediaData(std::shared_ptr<uint8_t>, size_t);
    MediaData(const MediaData &) = delete;
    std::shared_ptr<uint8_t> buf_;
    size_t size_ = 0;
};

enum class ColorRange { kUnknown, kLimited, kFull };

enum class ColorSpace { kUnknown, kBT601, kBT709, kBT2020 };

class RenderContext;

class Bitmap {
public:
    Bitmap() {}
    virtual ~Bitmap() {}

    virtual uint8_t *GetNativePtr() = 0;
    virtual void ReleaseNativePtr() = 0;
    virtual void *GetBitmap() = 0;
};

struct VideoFrame {
    // clang-format off
    enum class Type { 
        kUnknown, 
        kTexture, 
        kBitmap, 
        kPixelBuffer, 
        kRGBA, 
        kBGRA, 
        kARGB, 
        kI420, 
        kNV12, 
        kNV21,
        kJPEGData,
        kNdkImage,
    };

    enum class PixelbufferType {
        kUnknown,
        k420YpCbCr8BiPlanar, // NV12
        k420YpCbCr8Planar, // I420
        k32BGRA, // RGB
        k420YpCbCr10BiPlanar, //x420
    };

    enum class NdkImageFormat {
        kUnknown,
        kI420,
        kNV12,
        kNV21,
        kJPEG,
    };

    // clang-format on

    struct Crop {
        float x = -1.0f;
        float y = -1.0f;
        float width = -1.0f;
        float height = -1.0f;

        Crop() : x(-1.0f), y(-1.0f), width(-1.0f), height(-1.0f) {}

        Crop(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

        bool IsValid() const { return x >= 0.0f && y >= 0.0f && width >= 0.0f && height >= 0.0f; }
    };

    struct Attributes {
        bool from_front_camera = false;
        bool mirror = false;
        int32_t rotation = 0;
        // 目标宽，包括旋转、裁剪后的
        uint32_t output_width = 0;
        // 目标高，包括旋转、裁剪后的高
        uint32_t output_height = 0;
        bool is_captured = false;
        ColorRange color_range = ColorRange::kUnknown;
        ColorSpace color_space = ColorSpace::kUnknown;
        float clarity = 1.0f;
        // 旋转之前的裁剪区域，原始图像上的裁剪区域，不包括旋转
        Crop crop;
        float zoom_factor = 1.0f;
        std::string texture_signature;
        std::vector<std::shared_ptr<Watermark>> watermarks;

        // TruePhoto
        std::vector<TruePhotoInfo> truephoto_infos;
        TruePhotoErrorCode truephoto_error_code = TruePhotoErrorCode::kSuccess;

        Attributes()
            : from_front_camera(false), mirror(false), rotation(0), is_captured(false), color_range(ColorRange::kUnknown),
              color_space(ColorSpace::kUnknown), clarity(1.0f), zoom_factor(1.0f), texture_signature("") {}

        Attributes(bool from_front_camera, bool mirror, int32_t rotation, uint32_t output_width, uint32_t output_height,
                   bool is_captured, ColorRange color_range, ColorSpace color_space, float clarity, float zoom_factor,
                   const std::string &texture_signature)
            : from_front_camera(from_front_camera), mirror(mirror), rotation(rotation), output_width(output_width),
              output_height(output_height), is_captured(is_captured), color_range(color_range), color_space(color_space),
              clarity(clarity), zoom_factor(zoom_factor), texture_signature(texture_signature) {}

        void ClearTransform() {
            output_width = 0;
            output_height = 0;
            rotation = 0;
            mirror = false;
            crop.x = -1.0f;
            crop.y = -1.0f;
            crop.width = -1.0f;
            crop.height = -1.0f;
            zoom_factor = 1.0f;
            texture_signature = "";
        }

        bool HasTransform() const {
            return output_width > 0 || output_height > 0 || rotation != 0 || crop.IsValid() || zoom_factor != 1.0f || mirror;
        }
    };

    std::shared_ptr<VideoFrame> Clone() {
        std::shared_ptr<VideoFrame> clone = std::make_shared<VideoFrame>();
        clone->attributes = attributes;
        clone->type = type;
        clone->pixel_buffer_type = pixel_buffer_type;
        clone->texture = texture;
        clone->pixel_buffer = pixel_buffer;
        clone->bitmap = bitmap;
        clone->width = width;
        clone->height = height;
        clone->timestamp = timestamp;
        clone->original_frame = original_frame;
        return clone;
    }

    Attributes attributes;
    Type type;
    PixelbufferType pixel_buffer_type;
    NdkImageFormat ndk_image_format = NdkImageFormat::kUnknown;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<void> pixel_buffer;
    std::shared_ptr<Bitmap> bitmap;
    uint32_t width;
    uint32_t height;
    // 单位微妙
    uint64_t timestamp;
    uint32_t linesize[3];
    std::unique_ptr<MediaData> buffer[3];
    std::shared_ptr<VideoFrame> original_frame;

    const std::shared_ptr<Texture> &GetYTex() const { return y_tex_; }
    const std::shared_ptr<Texture> &GetUTex() const { return u_tex_; }
    const std::shared_ptr<Texture> &GetVTex() const { return v_tex_; }
    const std::shared_ptr<Texture> &GetUVTex() const { return uv_tex_; }
    const std::shared_ptr<Texture> &GetRGBATex() const { return rgba_tex_; }

    std::shared_ptr<Texture> GetYTex(RenderContext *render_context);
    std::shared_ptr<Texture> GetUTex(RenderContext *render_context);
    std::shared_ptr<Texture> GetVTex(RenderContext *render_context);
    std::shared_ptr<Texture> GetUVTex(RenderContext *render_context);
    std::shared_ptr<Texture> GetRGBATex(RenderContext *render_context);

    int GetTransformedWidth() const {
        int32_t width = (attributes.rotation % 180 == 0) ? this->width : this->height;
        if (!attributes.crop.IsValid()) {
            return width;
        }

        return (attributes.rotation % 180 == 0) ? width * attributes.crop.width : width * attributes.crop.height;
    }

    int GetTransformedHeight() const {
        int32_t height = (attributes.rotation % 180 == 0) ? this->height : this->width;
        if (!attributes.crop.IsValid()) {
            return height;
        }

        return (attributes.rotation % 180 == 0) ? height * attributes.crop.height : height * attributes.crop.width;
    }

    int GetOutputWidth() const { return attributes.output_width > 0 ? attributes.output_width : GetTransformedWidth(); }
    int GetOutputHeight() const { return attributes.output_height > 0 ? attributes.output_height : GetTransformedHeight(); }

    bool CanDrop() { return !attributes.is_captured; }

private:
    void ExtractPixelbufferTextures(RenderContext *render_context);
    void ExtractOriginalFrameIfNeeded();

private:
    std::shared_ptr<Texture> y_tex_;
    std::shared_ptr<Texture> u_tex_;
    std::shared_ptr<Texture> v_tex_;
    std::shared_ptr<Texture> uv_tex_;
    std::shared_ptr<Texture> rgba_tex_;
};

enum AudioSampleFormatType {
    kSampleFormatTypeUnknown = 0,
    kSampleFormatTypeU8 = 1,
    kSampleFormatTypeS16LE = 2,
    kSampleFormatTypeS24LE = 3,
    kSampleFormatTypeS32LE = 4,
    kSampleFormatTypeF32LE = 5,
    kSampleFormatTypeU8P = 6,
    kSampleFormatTypeS16P = 7,
    kSampleFormatTypeS24P = 8,
    kSampleFormatTypeS32P = 9,
    kSampleFormatTypeF32P = 10
};

enum AudioChannelLayout {
    kAudioChannelLayoutUnknown = 0,
    kAudioChannelLayoutMono,
    kAudioChannelLayoutStereo,
};

// PCM_s16le
struct AudioFrame {
    enum Type {
        kUnknown,
        kPCM,
        kAac,
        kOpus,
    };
    std::unique_ptr<MediaData> buf;
    // 单位微妙
    int64_t timestamp;
    int32_t sample_rate;
    int32_t channel_num;
    AudioSampleFormatType sample_format;
    AudioChannelLayout channel_layout;
    Type type;
    bool is_recorded = false;

    int64_t DurationInMs() {
        return 1000.0 * buf->Size() / (sample_rate * channel_num * (sample_format == kSampleFormatTypeS16LE ? 2 : 4));
    }
};

} // namespace xhey::camera
