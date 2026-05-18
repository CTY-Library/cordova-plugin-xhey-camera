#pragma once

#include "defs.h"
#include "processor/processor_group.h"
#include "processor/workshop/cpu_thread_workshop.h"
#include "processor/workshop/render_thread_workshop.h"
#include "recorder/media_recorder.h"

namespace xhey::camera {

class MediaKit final : public std::enable_shared_from_this<MediaKit> {
public:
    MediaKit();
    ~MediaKit();

    bool Init(const MediaKitConfig &config);

    void Flush(std::function<void()> callback);

    void Release();

    std::shared_ptr<MediaRecorder> GetMediaRecorder();

    bool AddVideoProcessorToGroup(const std::shared_ptr<Processor<std::shared_ptr<VideoFrame>>> &processor,
                                  ProcessorGroupType group, bool to_front);
    bool RemoveVideoProcessorFromGroup(const std::shared_ptr<Processor<std::shared_ptr<VideoFrame>>> &processor,
                                       ProcessorGroupType group);

    bool AddAudioProcessorToGroup(const std::shared_ptr<Processor<std::shared_ptr<AudioFrame>>> &processor,
                                  ProcessorGroupType group);
    bool RemoveAudioProcessorFromGroup(const std::shared_ptr<Processor<std::shared_ptr<AudioFrame>>> &processor,
                                       ProcessorGroupType group);

    void OnVideoFrame(const std::shared_ptr<VideoFrame> &video_frame);
    void OnAudioFrame(const std::shared_ptr<AudioFrame> &audio_frame);

private:
    std::shared_ptr<EglMediaThread> cpu_thread_;
    std::shared_ptr<EglMediaThread> render_thread_;
    std::shared_ptr<EglMediaThread> capture_image_thread_;

    std::shared_ptr<VideoProcessorGroup> cpu_processor_group_;
    std::shared_ptr<VideoProcessorGroup> gpu_processor_group_;
    std::shared_ptr<VideoProcessorGroup> preview_processor_group_;
    std::shared_ptr<VideoProcessorGroup> record_processor_group_;
    std::shared_ptr<VideoProcessorGroup> capture_image_processor_group_;

    std::shared_ptr<AudioProcessorGroup> audio_processor_group_;

    std::shared_ptr<MediaPublisher<std::shared_ptr<VideoFrame>>> video_source_;
    std::shared_ptr<MediaPublisher<std::shared_ptr<AudioFrame>>> audio_source_;

    MediaKitConfig config_;

    std::shared_ptr<CpuThreadWorkshop> cpu_thread_workshop_;
    std::shared_ptr<RenderThreadWorkshop> render_thread_workshop_;
    std::shared_ptr<RenderThreadWorkshop> capture_image_thread_workshop_;

    std::shared_ptr<MediaRecorder> media_recorder_;
};

} // namespace xhey::camera
