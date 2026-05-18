#pragma once

#include "gl/gl_thread_attacher.h"
#include <memory>

namespace xhey::camera {

enum class ProcessorGroupType {
    kProcessorGroupCpu = 0,
    kProcessorGroupGpu = 1,
    kProcessorGroupPreview = 2,
    kProcessorGroupRecord = 3,
    kProcessorGroupCaptureImage = 4,
    kProcessorGroupAudio = 5,
};

struct MediaKitConfig {
    bool enable_cpu_thread = false;
    bool enable_media_recorder = false;
    bool enable_capture_image_thread = false;
    std::shared_ptr<GlThreadAttacherFactory> gl_thread_attacher_factory;
};

} // namespace xhey::camera
