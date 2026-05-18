#pragma once

#include "common/error.h"
#include "processor/processor.h"

namespace xhey::camera {

class DataExtractProcessor : public Processor<std::shared_ptr<VideoFrame>>,
                             public std::enable_shared_from_this<DataExtractProcessor> {
public:
    enum TriggerMode {
        kTriggerModeAlways = 0,
        kTriggerModeRequested = 1,
    };

    enum FrameType {
        kFrameTypeTexture = 0,
        kFrameTypeRGBA = 1,
        kFrameTypePixelBuffer = 2,
        kFrameTypeBitmap = 3,
    };

    DataExtractProcessor();
    ~DataExtractProcessor();

    void SetTriggerMode(TriggerMode mode);
    void ExtractOneFrame();
    void SetFrameType(FrameType type);
    void SetDataCallback(const std::function<void(const std::shared_ptr<VideoFrame> &, Error error)> &callback);

    std::shared_ptr<VideoFrame> Process(const std::shared_ptr<VideoFrame> &input) override;

private:
    void ExtractTextureFrame(const std::shared_ptr<VideoFrame> &frame);
    void ExtractRGBAFrame(const std::shared_ptr<VideoFrame> &frame);
    void ExtractPixelBufferFrame(const std::shared_ptr<VideoFrame> &frame);

private:
    TriggerMode trigger_mode_ = kTriggerModeAlways;
    bool requested_ = false;
    FrameType frame_type_ = kFrameTypeRGBA;
    std::function<void(const std::shared_ptr<VideoFrame> &, Error error)> data_callback_;
};

} // namespace xhey::camera
