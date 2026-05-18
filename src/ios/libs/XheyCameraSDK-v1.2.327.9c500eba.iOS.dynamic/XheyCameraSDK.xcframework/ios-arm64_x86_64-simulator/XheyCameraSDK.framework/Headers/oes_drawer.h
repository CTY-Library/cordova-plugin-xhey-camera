#pragma once

#include "common/media_types.h"
#include "gl/shader_program.h"

namespace xhey::camera {

class RenderContext;

class OesDrawer {
public:
    OesDrawer(RenderContext *render_context);
    ~OesDrawer();

    void Draw(VideoFrame *video_frame, bool render_to_texture);
    void Draw(VideoFrame *video_frame, ViewPort view_port, bool render_to_texture);

private:
    std::unique_ptr<ShaderProgram> program_;
    GLint position_attribute_location_ = -1;
    GLint texture_coordinate_attribute_location_ = -1;
    GLint mvp_matrix_uniform_location_ = -1;
    GLint input_image_texture_uniform_location_ = -1;
};

} // namespace xhey::camera
