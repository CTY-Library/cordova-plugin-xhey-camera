#pragma once

#include "common/egl_media_thread.h"
#include "common/error.h"
#include "common/media_types.h"
#include "defs.h"

namespace xhey::camera {

struct VideoInfo {
    uint32_t width;
    uint32_t height;
    std::shared_ptr<VideoFrame> preview_image;
    double duration;
    std::string file_path;
};

class MediaRecorderListener {
public:
    virtual ~MediaRecorderListener() = default;
    virtual void OnStart() = 0;
    virtual void OnStop(const VideoInfo &video_info) = 0;
    virtual void OnProgress(double progress) = 0;
    virtual void OnError(const Error &error) = 0;
};

class IMediaRecorder {
public:
    IMediaRecorder();
    virtual ~IMediaRecorder() = default;

    virtual void Init(const MediaKitConfig &config, const std::shared_ptr<EglMediaThread> &render_thread) = 0;
    virtual void Start(const std::string &file_path, uint32_t width, uint32_t height,
                       const std::shared_ptr<MediaRecorderListener> &listener) = 0;
    virtual void Stop() = 0;
    virtual void OnVideoFrame(const std::shared_ptr<VideoFrame> &video_frame) = 0;
    virtual void OnAudioFrame(const std::shared_ptr<AudioFrame> &audio_frame) = 0;
    virtual bool IsRecording() const = 0;
    virtual void Release() = 0;
};

class MediaRecorder final {
public:
    MediaRecorder();
    ~MediaRecorder();

    void Init(const MediaKitConfig &config, const std::shared_ptr<EglMediaThread> &render_thread);
    void Start(const std::string &file_path, uint32_t width, uint32_t height,
               const std::shared_ptr<MediaRecorderListener> &listener);
    void Stop();

    void OnVideoFrame(const std::shared_ptr<VideoFrame> &video_frame);
    void OnAudioFrame(const std::shared_ptr<AudioFrame> &audio_frame);

    bool IsRecording() const;

    void Release();

private:
    MediaKitConfig config_;
    std::string file_path_;
    std::shared_ptr<MediaRecorderListener> listener_;
    int32_t video_width_ = 0;
    int32_t video_height_ = 0;
    std::unique_ptr<IMediaRecorder> impl_;
};

} // namespace xhey::camera
