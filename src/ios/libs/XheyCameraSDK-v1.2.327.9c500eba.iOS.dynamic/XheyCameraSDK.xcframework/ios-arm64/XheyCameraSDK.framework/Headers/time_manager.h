#pragma once

#include "common/misc_utils.h"
#include "network/retry_manager.h"
#include <functional>

namespace xhey::camera {

class TimeManager : public Singleton<TimeManager> {
public:
    // 单位毫秒
    int64_t GetCurrentTime();
    bool GetInChina() const;
    void Refresh(std::function<void(int64_t, bool)> callback);
    void Cancel();

private:
    TimeManager();
    ~TimeManager();
    void RefreshImpl();
    void OnTimestamp(int64_t timstamp, bool in_china);

private:
    friend class Singleton<TimeManager>;

private:
    mutable std::mutex mutex_;
    int64_t current_time_ = -1;
    int64_t boot_time_ = -1;
    bool in_china_ = true;
    std::function<void(int64_t, bool)> refresh_callback_;
    std::shared_ptr<RetryManager> retry_manager_;
};

} // namespace xhey::camera
