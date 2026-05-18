#pragma once

#include "common/media_types.h"
#include "gl/shader_program.h"

namespace xhey::camera {

class RenderContext;

class RgbaDrawer {
public:
    RgbaDrawer(RenderContext *render_context);
    ~RgbaDrawer();

    void Draw(VideoFrame *video_frame, bool render_to_texture);
    void Draw(VideoFrame *video_frame, ViewPort view_port, bool render_to_texture);

    static bool CanDraw(VideoFrame *video_frame) {
        return video_frame->type == VideoFrame::Type::kRGBA || video_frame->type == VideoFrame::Type::kBGRA ||
               video_frame->type == VideoFrame::Type::kARGB ||
               (video_frame->type == VideoFrame::Type::kPixelBuffer &&
                video_frame->pixel_buffer_type == VideoFrame::PixelbufferType::k32BGRA);
    }

private:
    RenderContext *render_context_ = nullptr;
    std::unique_ptr<ShaderProgram> program_;
    GLint position_attribute_location_ = -1;
    GLint texture_coordinate_attribute_location_ = -1;
    GLint mvp_matrix_uniform_location_ = -1;
    GLint input_image_texture_uniform_location_ = -1;
    GLint format_uniform_location_ = -1;
};

} // namespace xhey::camera
