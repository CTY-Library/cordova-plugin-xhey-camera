#pragma once

#include "common/media_types.h"

namespace xhey::camera {

class TextureReader {
public:
    TextureReader(RenderContext *render_context);
    ~TextureReader();

    void ReadTexture(const std::shared_ptr<VideoFrame> &video_frame, uint8_t *data);

private:
    RenderContext *render_context_ = nullptr;
};

} // namespace xhey::camera
