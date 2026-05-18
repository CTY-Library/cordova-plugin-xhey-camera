#pragma once

#include "common/media_types.h"

namespace xhey::camera {

class VideoUtils {
public:
    static std::shared_ptr<VideoFrame> ExtractFrameFromPixelbuffer(VideoFrame *frame);
};
} // namespace xhey::camera
