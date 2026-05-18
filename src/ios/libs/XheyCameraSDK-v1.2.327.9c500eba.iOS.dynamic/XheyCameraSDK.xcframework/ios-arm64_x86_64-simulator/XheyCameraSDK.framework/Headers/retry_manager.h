#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>

// 前向声明
namespace xhey {
namespace camera {
class Runloop;
}
} // namespace xhey

namespace xhey {
namespace camera {

class RetryManager : public std::enable_shared_from_this<RetryManager> {
public:
    using RetryCallback = std::function<void()>;

    RetryManager(Runloop *runloop);
    ~RetryManager();

    // 设置重试回调函数
    void SetRetryCallback(RetryCallback callback);

    // 开始重试
    void StartRetry();

    // 停止重试
    void StopRetry();

    // 重置重试状态（成功或手动连接时调用）
    void ResetRetryState();

    // 检查是否正在重试
    bool IsRetrying() const;

    // 获取当前重试次数
    int GetCurrentRetryCount() const;

    // 获取当前重试间隔（毫秒）
    int GetCurrentRetryInterval() const;

    // 设置最大重试次数，超过后不再重试（0 表示不限制）
    void SetMaxRetryCount(int32_t max_count);

    // 获取最大重试次数
    int32_t GetMaxRetryCount() const;

private:
    void ScheduleNextRetry();
    void ExecuteRetry();

    // 重试策略配置
    struct RetryPhase {
        int32_t max_attempts;     // 最大尝试次数
        int32_t interval_ms;      // 重试间隔（毫秒）
        int32_t current_attempts; // 当前已尝试次数
    };

    RetryCallback retry_callback_;
    std::atomic<bool> is_retrying_;
    std::atomic<int32_t> current_retry_count_;
    std::atomic<int32_t> current_phase_index_;
    std::atomic<int32_t> max_retry_count_; // 最大重试次数，0 表示不限制

    // 重试阶段配置
    static constexpr int32_t kMaxPhases = 5;
    RetryPhase retry_phases_[kMaxPhases];

    mutable std::mutex mutex_;

    // Runloop 引用
    Runloop *runloop_;
};

} // namespace camera
} // namespace xhey
