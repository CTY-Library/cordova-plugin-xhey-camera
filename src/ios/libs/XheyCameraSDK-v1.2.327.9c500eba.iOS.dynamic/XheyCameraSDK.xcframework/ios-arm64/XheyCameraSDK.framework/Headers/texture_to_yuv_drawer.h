#pragma once

#include "common/media_types.h"
#include "gl/shader_program.h"

namespace xhey::camera {

class RenderContext;

class TextureToYuvDrawer {
public:
    TextureToYuvDrawer(RenderContext *render_context);
    ~TextureToYuvDrawer();

    void Draw(VideoFrame *video_frame, bool render_to_texture);
    void Draw(VideoFrame *video_frame, ViewPort view_port, bool render_to_texture);

    static bool CanDraw(VideoFrame *video_frame) { return video_frame->type == VideoFrame::Type::kTexture; }

private:
    RenderContext *render_context_ = nullptr;
    std::unique_ptr<ShaderProgram> program_;
    GLint position_attribute_location_ = -1;
    GLint texture_coordinate_attribute_location_ = -1;
    GLint mvp_matrix_uniform_location_ = -1;
    GLint input_image_texture_uniform_location_ = -1;
    GLint color_conversion_uniform_location_ = -1;
};

} // namespace xhey::camera
