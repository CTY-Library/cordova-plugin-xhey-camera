#pragma once

#include "common/media_types.h"
#include "gl/shader_program.h"

namespace xhey::camera {

class RenderContext;

class YuvDrawer {
public:
    YuvDrawer(RenderContext *render_context);
    ~YuvDrawer();

    void Draw(VideoFrame *video_frame, bool render_to_texture);
    void Draw(VideoFrame *video_frame, ViewPort view_port, bool render_to_texture);

    static bool CanDraw(VideoFrame *video_frame) {
        return video_frame->type == VideoFrame::Type::kI420 || video_frame->type == VideoFrame::Type::kNV12 ||
               video_frame->type == VideoFrame::Type::kNV21 ||
               (video_frame->type == VideoFrame::Type::kPixelBuffer &&
                (video_frame->pixel_buffer_type == VideoFrame::PixelbufferType::k420YpCbCr8Planar ||
                 video_frame->pixel_buffer_type == VideoFrame::PixelbufferType::k420YpCbCr8BiPlanar)) ||
               video_frame->type == VideoFrame::Type::kNdkImage;
    }

private:
    RenderContext *render_context_ = nullptr;
    std::unique_ptr<ShaderProgram> program_;
    GLint position_attribute_location_ = -1;
    GLint texture_coordinate_attribute_location_ = -1;
    GLint mvp_matrix_uniform_location_ = -1;
    GLint format_uniform_location_ = -1;
    GLint texture0_uniform_location_ = -1;
    GLint texture1_uniform_location_ = -1;
    GLint texture2_uniform_location_ = -1;
    GLint yoffset_uniform_location_ = -1;
    GLint color_conversion_uniform_location_ = -1;
};

} // namespace xhey::camera
