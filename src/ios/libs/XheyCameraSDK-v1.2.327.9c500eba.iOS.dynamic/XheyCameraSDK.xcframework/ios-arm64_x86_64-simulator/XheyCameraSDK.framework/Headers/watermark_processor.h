#pragma once

#include "common/media_types.h"
#include "gl/shader_program_pool.h"
#include "gl/texture_reader.h"
#include "processor/processor.h"
#include <cstdint>
#include <map>

namespace xhey {
namespace camera {

class WatermarkProcessor : public Processor<std::shared_ptr<VideoFrame>>,
                           public std::enable_shared_from_this<WatermarkProcessor> {
public:
    WatermarkProcessor();
    ~WatermarkProcessor();

    void Release();

    std::shared_ptr<VideoFrame> Process(const std::shared_ptr<VideoFrame> &input) override;

    void AddWatermark(const std::shared_ptr<Watermark> &watermark);

    void SetRotation(int32_t rotation);

    void SetMirrorFrontCameraOutput(bool mirror);

    void AutoClearWatermarkAfterCapture(bool auto_clear);

    void ClearAllWatermarks();

private:
    std::shared_ptr<VideoFrame> ProcessVideoFrame(const std::shared_ptr<VideoFrame> &video_frame);
    std::shared_ptr<VideoFrame> ProcessImageFrame(const std::shared_ptr<VideoFrame> &video_frame);

private:
    std::map<uint32_t, std::shared_ptr<Watermark>> watermark_map_;
    uint32_t rotation_ = 0;
    bool mirror_front_camera_output_ = true;
    bool auto_clear_watermark_after_capture_ = true;
};

} // namespace camera
} // namespace xhey
