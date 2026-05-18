#pragma once

#include "common/connectable.h"
#include "common/egl_media_thread.h"
#include "common/media_types.h"
#include "log/log.h"
#include "processor_wrapper.h"
#include "workshop/abstract_frame_drop_manager.h"
#include "workshop/easy_frame_drop_manager.h"
#include <deque>
#include <mutex>

namespace xhey::camera {

template <typename T>
class ProcessorGroup : public MediaWorkshop<T> {
public:
    ProcessorGroup(const std::shared_ptr<EglMediaThread> &egl_media_thread, const std::string &name)
        : egl_media_thread_(egl_media_thread), name_(name) {}

    ~ProcessorGroup() {}

    bool AddProcessor(const std::shared_ptr<Processor<T>> &processor, bool to_front) {
        if (ContainsProcessor(processor)) {
            XLOG_ERROR("Processor already exists in the group");
            assert(false);
            return false;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        auto processor_wrapper = std::make_shared<ProcessorWrapper<T>>(processor, egl_media_thread_);
        if (to_front) {
            processors_.push_front(processor_wrapper);
        } else {
            processors_.push_back(processor_wrapper);
        }
        return true;
    }

    bool AddTailProcessor(const std::shared_ptr<Processor<T>> &processor) {
        if (ContainsProcessor(processor)) {
            XLOG_ERROR("Processor already exists in the group");
            assert(false);
            return false;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        auto processor_wrapper = std::make_shared<ProcessorWrapper<T>>(processor, egl_media_thread_);
        tail_processors_.push_back(processor_wrapper);
        return true;
    }

    bool RemoveProcessor(const std::shared_ptr<Processor<T>> &processor) {
        processor->OnDisconnectFromPipeline();

        std::lock_guard<std::mutex> lock(mutex_);

        for (auto it = processors_.begin(); it != processors_.end(); ++it) {
            if ((*it)->GetProcessor() == processor) {
                processors_.erase(it);
                return true;
            }
        }

        for (auto it = tail_processors_.begin(); it != tail_processors_.end(); ++it) {
            if ((*it)->GetProcessor() == processor) {
                tail_processors_.erase(it);
                return true;
            }
        }

        return false;
    }

protected:
    void OnMediaArrived(const T &frame, uint32_t source_id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        T currrent_frame = frame;

        for (const auto &wrapper : processors_) {
            if (!wrapper->GetProcessor()->IsEnable()) {
                continue;
            }

            currrent_frame = wrapper->Process(currrent_frame);
        }

        for (const auto &wrapper : tail_processors_) {
            if (!wrapper->GetProcessor()->IsEnable()) {
                continue;
            }

            currrent_frame = wrapper->Process(currrent_frame);
        }

        this->Publish(currrent_frame);
    }

private:
    bool ContainsProcessor(const std::shared_ptr<Processor<T>> &processor) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto &wrapper : processors_) {
            if (wrapper->GetProcessor() == processor) {
                return true;
            }
        }

        for (const auto &wrapper : tail_processors_) {
            if (wrapper->GetProcessor() == processor) {
                return true;
            }
        }

        return false;
    }

private:
    std::mutex mutex_;
    std::deque<std::shared_ptr<ProcessorWrapper<T>>> processors_;
    std::deque<std::shared_ptr<ProcessorWrapper<T>>> tail_processors_;
    std::shared_ptr<EglMediaThread> egl_media_thread_;
    std::string name_;
};

using VideoProcessorGroup = ProcessorGroup<std::shared_ptr<VideoFrame>>;
using AudioProcessorGroup = ProcessorGroup<std::shared_ptr<AudioFrame>>;

} // namespace xhey::camera
