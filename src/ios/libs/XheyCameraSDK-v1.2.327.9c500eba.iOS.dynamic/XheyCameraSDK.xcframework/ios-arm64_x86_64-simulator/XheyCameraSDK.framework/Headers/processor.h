#pragma once

#include "common/egl_media_thread.h"
#include <string>

namespace xhey::camera {

template <typename T>
class Processor {
public:
    Processor(const std::string &name) : name_(name) {}
    virtual ~Processor() = default;

    virtual T Process(const T &input) = 0;

    void SetEnable(bool enable) { enable_ = enable; }

    virtual bool IsEnable() const { return enable_; }

    void SetEglMediaThread(const std::shared_ptr<EglMediaThread> &egl_media_thread) {
        if constexpr (std::is_same_v<T, std::shared_ptr<VideoFrame>>) {
            // 只有当 T 是 std::shared_ptr<VideoFrame> 时才执行这个断言
            assert(egl_media_thread_ == nullptr && egl_media_thread != nullptr);
        }
        egl_media_thread_ = egl_media_thread;
    }

    virtual void OnConnectToPipeline() { XLOG_INFO("Processor %s connected to pipeline", name_.c_str()); }

    virtual void OnDisconnectFromPipeline() {}

    const std::string &Name() const { return name_; }

    void PostToGlThreadIfConnectedOrRun(const std::function<void()> &func) {
        if (egl_media_thread_ && (egl_media_thread_->ThreadId() != std::this_thread::get_id())) {
            egl_media_thread_->Post(func);
        } else {
            func();
        }
    }

    bool IsWrapped() const { return wrapped_; }
    void SetWrapped(bool wrapped) { wrapped_ = wrapped; }

protected:
    std::shared_ptr<EglMediaThread> egl_media_thread_;
    std::string name_;
    std::atomic<bool> enable_{false};
    bool wrapped_ = false;
};

} // namespace xhey::camera
