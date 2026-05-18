#pragma once

#include "misc_utils.h"
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace xhey {
namespace camera {

class ThreadPool : public NonCopyable {
public:
    static ThreadPool &DefaultThreadPool();

    explicit ThreadPool(size_t size);

    explicit ThreadPool(size_t size, std::function<void(int)> on_thread_create, std::function<void(int)> on_thread_destroy);

    ~ThreadPool();

    bool Post(std::function<void()> task);

private:
    void ExecuteTasks(int index);

    void StartThreads(size_t size);

    std::vector<std::shared_ptr<std::thread>> pool_;
    std::queue<std::shared_ptr<std::function<void()>>> task_queue_;
    std::mutex mutex_;
    std::condition_variable task_cond_;
    std::atomic<bool> stop_;
    std::function<void(int)> on_thread_create_;
    std::function<void(int)> on_thread_destroy_;
};

} // namespace camera
} // namespace xhey
