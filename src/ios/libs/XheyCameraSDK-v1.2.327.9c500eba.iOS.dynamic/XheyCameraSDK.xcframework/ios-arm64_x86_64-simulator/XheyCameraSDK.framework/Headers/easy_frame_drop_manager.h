#pragma once

#include "abstract_frame_drop_manager.h"

namespace xhey::camera {

template <class T, class = void>
struct CanDropHelper;

template <class MediaType>
class EasyFrameDropManager : public AbstractFrameDropManager<MediaType> {
public:
    EasyFrameDropManager(int max_queue_size) : max_size_(max_queue_size) {}

    virtual void
    SetFrameDropCallback(const std::function<void(const std::shared_ptr<MediaType> &, bool from_input)> &callback) override {
        std::lock_guard<std::mutex> lg(lock_);
        frame_drop_callback_ = callback;
    }

    virtual bool InputVideoFrame(const std::shared_ptr<MediaType> &frame) override {
        std::lock_guard<std::mutex> lg(lock_);

        if ((drop_all_enabled_ || queue_.size() >= max_size_) && CanDropHelper<MediaType>::CanDrop(frame)) {
            if (frame_drop_callback_) {
                frame_drop_callback_(frame, true);
            }

            return false;
        }

        queue_.push_back(frame);
        if (frame_count_changed_callback_) {
            frame_count_changed_callback_(queue_.size());
        }

        return true;
    }

    virtual std::shared_ptr<MediaType> OutputVideoFrame() override {
        std::lock_guard<std::mutex> lg(lock_);

        if (queue_.empty()) {
            return nullptr;
        }

        auto frame = queue_.front();
        queue_.pop_front();
        if (frame_count_changed_callback_) {
            frame_count_changed_callback_(queue_.size());
        }

        return frame;
    }

    void SetFrameCountChangedCallback(const std::function<void(int64_t)> &callback) override {
        std::lock_guard<std::mutex> lg(lock_);
        frame_count_changed_callback_ = callback;
    }

    virtual void DrainFrameUntilSize(uint32_t size) override {
        std::lock_guard<std::mutex> lg(lock_);

        while (queue_.size() > size) {
            auto frame = queue_.front();
            queue_.pop_front();

            if (frame_drop_callback_) {
                frame_drop_callback_(frame, false);
            }
        }
    }

    virtual void SetDropAllEnabled(bool enabled) override {
        std::lock_guard<std::mutex> lg(lock_);
        drop_all_enabled_ = enabled;

        if (!drop_all_enabled_) {
            return;
        }

        // 将队列里所有可丢弃的帧都设置为nullptr，但是不改变队列里帧的数量，防止OutputVideoFrame的帧序与命令的序列错乱
        for (int i = 0; i < queue_.size(); ++i) {
            if (queue_[i] && CanDropHelper<MediaType>::CanDrop(queue_[i])) {
                if (frame_drop_callback_) {
                    frame_drop_callback_(queue_[i], false);
                }

                queue_[i] = nullptr;
            }
        }
    }

    void Clear() override {
        std::lock_guard<std::mutex> lg(lock_);
        queue_.clear();
    }

private:
    std::mutex lock_;
    std::deque<std::shared_ptr<MediaType>> queue_;
    size_t max_size_;
    std::function<void(std::shared_ptr<MediaType>, bool from_input)> frame_drop_callback_;
    bool drop_all_enabled_ = false;
    std::function<void(int64_t)> frame_count_changed_callback_;
    friend class CanDropHelper<MediaType>;
};

template <class T>
struct CanDropHelper<T, typename std::enable_if<std::is_same<T, VideoFrame>::value>::type> {
    static bool CanDrop(std::shared_ptr<T> t) { return t->CanDrop(); }
};

template <class T>
struct CanDropHelper<T, typename std::enable_if<std::is_same<T, AudioFrame>::value>::type> {
    static bool CanDrop(std::shared_ptr<T> t) { return false; }
};

} // namespace xhey::camera
