#pragma once
#include "abstract_frame_drop_manager.h"
#include "log/log.h"
#include <map>

namespace xhey::camera {

class FrameDropManagerObserverManager {
public:
    FrameDropManagerObserverManager() = default;
    ~FrameDropManagerObserverManager() = default;

    void AddFrameDropManager(AbstractFrameDropManager<VideoFrame> *manager) {
        manager->SetFrameCountChangedCallback([=](int64_t frame_count) {
            {
                std::lock_guard<std::mutex> lg(mutex_);
                frame_count_map_[reinterpret_cast<int64_t>(manager)] = frame_count;
            }
            NotifyFrameCountChanged();
        });
    }

    void RemoveFrameDropManager(AbstractFrameDropManager<VideoFrame> *manager) {
        {
            std::lock_guard<std::mutex> lg(mutex_);
            frame_count_map_.erase(reinterpret_cast<int64_t>(manager));
        }
        NotifyFrameCountChanged();
    }

    void AddFrameDropManager(AbstractFrameDropManager<AudioRawFrame> *manager) {
        manager->SetFrameCountChangedCallback([=](int64_t frame_count) {
            {
                std::lock_guard<std::mutex> lg(mutex_);
                frame_count_map_[reinterpret_cast<int64_t>(manager)] = frame_count;
            }
            NotifyFrameCountChanged();
        });
    }

    void RemoveFrameDropManager(AbstractFrameDropManager<AudioRawFrame> *manager) {
        {
            std::lock_guard<std::mutex> lg(mutex_);
            frame_count_map_.erase(reinterpret_cast<int64_t>(manager));
        }
        NotifyFrameCountChanged();
    }

    void SetFrameCountChangedCallback(const std::function<void(int64_t)> &callback) {
        std::lock_guard<std::mutex> lg(mutex_);
        frame_count_changed_callback_ = callback;
    }

    void RemoveFrameCountChangedCallback() {
        std::lock_guard<std::mutex> lg(mutex_);
        frame_count_changed_callback_ = nullptr;
    }

    void NotifyFrameCountChanged() {
        std::lock_guard<std::mutex> lg(mutex_);
        int64_t max_count = 0;
        for (auto &pair : frame_count_map_) {
            max_count = std::max(max_count, pair.second);
            //            XLOG_INFO("manager: %p, frame count: %ld", pair.first, pair.second);
        }
        if (frame_count_changed_callback_) {
            frame_count_changed_callback_(max_count);
        }
    }

    int64_t GetMaxFrameCount() {
        std::lock_guard<std::mutex> lg(mutex_);
        int64_t max_count = 0;
        for (auto &pair : frame_count_map_) {
            max_count = std::max(max_count, pair.second);
        }
        return max_count;
    }

private:
    std::map<int64_t, int64_t> frame_count_map_;
    std::mutex mutex_;
    std::function<void(int64_t)> frame_count_changed_callback_;
};

} // namespace xhey::camera
