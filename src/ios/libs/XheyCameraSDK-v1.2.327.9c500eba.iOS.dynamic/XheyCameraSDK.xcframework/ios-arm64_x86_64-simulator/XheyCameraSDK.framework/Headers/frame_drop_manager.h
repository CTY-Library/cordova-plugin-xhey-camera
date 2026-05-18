#pragma once
#include "abstract_frame_drop_manager.h"
#include "common/media_types.h"
#include <deque>
#include <mutex>

namespace xhey::camera {

/**
 * Used for dropping frame crossing threads. This prevents continuous dropping frames.
 */
class FrameDropManager : public AbstractFrameDropManager<VideoFrame> {
public:
    FrameDropManager(int32_t max_continous_drop_frame = 1, int32_t min_drop_frame_queue_size = 2, int32_t max_queue_size = 3);
    virtual ~FrameDropManager();

    void SetFrameDropCallback(const std::function<void(const std::shared_ptr<VideoFrame> &, bool from_input)> &callback) override;

    // Input
    bool InputVideoFrame(const std::shared_ptr<VideoFrame> &) override;

    // Output, nullable
    std::shared_ptr<VideoFrame> OutputVideoFrame() override;

    void DrainFrameUntilSize(uint32_t size) override;

    void SetDropAllEnabled(bool enabled) override;

    void SetFrameCountChangedCallback(const std::function<void(int64_t)> &callback) override;

    void Clear() override;

private:
    struct DropFrameState {
        bool frame_dropped = false;
        int32_t continuous_drop_frame = 0;
    };

    void ResetDropFrameState();
    void FrameDropped(std::shared_ptr<VideoFrame> frame, bool from_input);

    int32_t max_continuous_drop_frame_ = 1;
    int32_t min_drop_frame_queue_size_ = 2;
    int32_t max_queue_size_ = 3;
    std::deque<std::shared_ptr<VideoFrame>> queue_;
    int queue_front_index_ = 0;
    std::mutex lock_;
    DropFrameState drop_frame_state_;
    std::function<void(int64_t)> frame_count_changed_callback_;
    std::function<void(std::shared_ptr<VideoFrame>, bool from_input)> frame_drop_callback_;
    bool input_dropped_;
    bool drop_all_enabled_;
};

} // namespace xhey::camera
